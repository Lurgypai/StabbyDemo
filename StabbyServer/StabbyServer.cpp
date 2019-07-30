// StabbyServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <fstream>

#include "enet/enet.h"

#include "SDL.h"
#include "NetworkTypes.h"
#include "PacketTypes.h"
#include "Controller.h"
#include <list>
#include <algorithm>
#include <vector>

#include "Connection.h"
#include "User.h"
#include "Stage.h"

#include "ServerClientData.h"
#include "Settings.h"
#include "PhysicsSystem.h"
#include "ZombieSpawner.h"
#include "CombatSystem.h"
#include "EntityBaseComponent.h"
#include "Host.h"

#define CLIENT_SIDE_DELTA 1.0 / 120

//I don't know how to copy peers, or If I should even try. Thus, to simplifiy things copying of connections is not allowed,
//to ensure peer->data isn't deleted multiple times

using UserPtr = std::unique_ptr<User>;
using std::ifstream;
using std::string;
using std::cout;

int main(int argv, char* argc[])
{
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_TIMER);
	cout << "Starting server...\n";
	enet_initialize();

	ifstream file{ "settings" };
	if (!file.good()) {
		cout << "Unable to open 'settings' file. Closing server.\n";
		return 1;
	}

	string line;
	Settings settings{ 0, 0, 0 };
	while (std::getline(file, line, '\n')) {
		size_t splitPos = line.find('=');
		string idToken = line.substr(0, splitPos);
		string value = line.substr(splitPos + 1, line.size() - splitPos);
		if (idToken == "port") {
			try {
				settings.port = std::stoi(value);
			}
			catch (std::invalid_argument e) {
				cout << "Unable to read port, closing server.\n";
				return 1;
			}
		}
		else if (idToken == "disconnectDelay") {
			try {
				settings.disconnectDelay = std::stod(value);
			}
			catch (std::invalid_argument e) {
				cout << "Unable to read disconnectDelay, closing server.\n";
				return 1;
			}
		}
		else if (idToken == "forceDisconnectDelay") {
			try {
				settings.forceDisconnectDelay = std::stod(value);
			}
			catch (std::invalid_argument e) {
				cout << "Unable to read forceDisconenctDelay, closing server.\n";
				return 1;
			}
		}
	}

	Host server;
	if (!server.createServer(settings.port, 32, 3)) {
		std::cout << "Oops, no server! Perhaps one is already running on this port?\n";
		return -1;
	}

	std::cout << "Beginning server loop.\n";


	//wait this long to disconnect a client
	double disconnectDelay{ settings.disconnectDelay }; // 5.0
	//force a disconenct at this point
	double forceDisconnectDelay{ settings.forceDisconnectDelay }; // 10.0

	Uint64 prev = SDL_GetPerformanceCounter();

	ENetPacket * packet;
	WelcomePacket welcomePacket;

	//current tick in server time
	Time_t currentTick{ 0 };
	double serverDelta{ 0 };

	Time_t gameTime{ 0 };

	std::vector<ControllerPacket> ctrls;
	std::list<UserPtr> users;

	Stage stage{};
	PhysicsSystem physics{};
	physics.setStage(&stage);
	CombatSystem combat{};

	ZombieSpawner zombieSpawner{0};

	NetworkId clientId = 0;

	//switch game to tick at client speed, but only send updates out at server speed
	//test this, it looks like its done.
	while (true) {
		Time_t now = SDL_GetPerformanceCounter();
		if (static_cast<double>(now - prev) / SDL_GetPerformanceFrequency() >= CLIENT_TIME_STEP) {
			gameTime += CLIENT_TIME_STEP / GAME_TIME_STEP;
			prev = now;

			//std::cout << "Server tick, polling packets...\n";
			ENetEvent event;
			int packetsPolled = 0;
			while (server.service(&event, 0) > 0) {
				switch (event.type) {
				case ENET_EVENT_TYPE_CONNECT:
					std::cout << "Connection received.\n";
					clientId = server.addPeer(event.peer);
					welcomePacket.currentTick = gameTime;
					welcomePacket.netId = clientId;
					server.sendPacket<WelcomePacket>(event.peer, 0, welcomePacket);

					//notify all online players of a new players join
					for (auto& user : users) {
						//tell them about us
						JoinPacket join{};
						join.joinerId = clientId;
						server.sendPacket<JoinPacket>(user->getConnection()->getPeer(), 0, join);

						//tell us about them
						JoinPacket us{};
						us.joinerId = user->getNetId();
						server.sendPacket(event.peer, 0, us);
					}

					users.emplace_back(std::make_unique<User>(User{clientId, std::make_unique<Connection>(*event.peer, clientId, currentTick) }));
					break;
				case ENET_EVENT_TYPE_RECEIVE:
					if (event.packet->dataLength == 0) {
						std::cout << "Garbage packet received.\n";
					}
					else {
						NetworkId senderId = *static_cast<NetworkId *>(event.peer->data);
						User * sender = nullptr;
						for (auto& user : users) {
							if (user->getNetId() == senderId)
								sender = user.get();
						}

						if (sender == nullptr)
							std::cout << "Sender was nullptr.\n";
						sender->getConnection()->setLastPacket(currentTick);

						std::string key = PacketUtil::readPacketKey(event.packet);
						if (key == CONT_KEY) {
							ControllerPacket cont{};
							std::memcpy(&cont, event.packet->data, event.packet->dataLength);
							cont.unserialize();

							for (auto& user : users) {
								ServerPlayerLC& player = user->getPlayer();
								if (user->getNetId() == cont.netId) {
									ClientCommand comm{ Controller{ cont.state }, cont.time };
									player.bufferInput(comm);
								}
							}
						}
						else if (key == TIME_KEY) {
							TimestampPacket time{};
							PacketUtil::readInto<TimestampPacket>(time, event.packet);
							time.unserialize();

							time.gameTime = gameTime;
							time.serverTime = currentTick;
							server.sendPacket<TimestampPacket>(event.peer, 0, time);
							for (auto& user : users) {
								if (user->getNetId() == time.id) {
									user->getPlayer().clientTime = time.clientTime - 1;
								}
							}
						}
						
					}
					break;
				case ENET_EVENT_TYPE_DISCONNECT:
					{
						QuitPacket q;
						NetworkId peerNetId;
						std::memcpy(&peerNetId, event.peer->data, sizeof(peerNetId));
						q.id = peerNetId;
						std::cout << "Player " << peerNetId << " disconnected.\n";
						for (auto iter = users.begin(); iter != users.end(); ++iter) {
							auto& user = *iter;
							auto con = user->getConnection();
							if (user->getNetId() == peerNetId) {
								con->setShouldReset(true);
							}
							else {
								server.sendPacket<QuitPacket>(con->getPeer(), 0, q);
							}
						}
					}
					break;
				default:
					std::cout << "Event handled.\n";
				}
				++packetsPolled;
			}

			zombieSpawner.trySpawnZombies(gameTime);

			//move
			for (auto& user : users) {
				user->getPlayer().update(gameTime);
			}

			serverDelta += CLIENT_TIME_STEP / SERVER_TIME_STEP;

			if (serverDelta >= 1) {
				currentTick += round(serverDelta);
				serverDelta = 0;
				//tell all
				std::vector<StatePacket> states;
				states.reserve(users.size());
				for (auto& user : users) {

					StatePacket pos{};
					pos.state = user->getPlayerState().getPlayerState();
					pos.state.when = user->getPlayer().clientTime;
					pos.when = gameTime;
					pos.id = user->getNetId();
					pos.serialize();
					states.push_back(pos);
					pos.unserialize();
					pos.unserialize();
				}

				std::vector<ZombiePacket> zombieStates;
				if (EntitySystem::Contains<ZombieLC>()) {
					zombieStates.reserve(EntitySystem::GetPool<ZombieLC>().size() - EntitySystem::GetPool<ZombieLC>().freeIndices());
					for (auto& zombie : EntitySystem::GetPool<ZombieLC>()) {
						ZombiePacket packet;
						packet.onlineId = zombie.onlineId;
						packet.state = zombie.getState();
						packet.isDead = EntitySystem::GetComp<EntityBaseComponent>(zombie.getId())->isDead;
						packet.serialize();
						zombieStates.push_back(packet);
					}
				}

				for (auto& other : users) {
					//send the contiguous state block
					server.sendData(other->getConnection()->getPeer(), 1, states.data(), sizeof(StatePacket) * states.size());
					if (EntitySystem::Contains<ZombieLC>()) {
						Time_t clientTime = htonll(other->getPlayer().clientTime);
						enet_uint8 * packet = static_cast<enet_uint8 *>(malloc(sizeof(Time_t) + sizeof(ZombiePacket) * zombieStates.size()));
						memcpy(packet, zombieStates.data(), sizeof(ZombiePacket) * zombieStates.size());
						memcpy(packet + (sizeof(ZombiePacket) * zombieStates.size()), &clientTime, sizeof(Time_t));

						server.sendData(other->getConnection()->getPeer(), 2, packet, sizeof(Time_t) + sizeof(ZombiePacket) * zombieStates.size());
						free(packet);
					}
				}

				//cleanup after server update, so that all "dead" states can be sent
				EntitySystem::FreeDeadEntities();
			}

			if (EntitySystem::Contains<ZombieLC>()) {
				for (auto& zombie : EntitySystem::GetPool<ZombieLC>()) {
					zombie.searchForTarget<ServerPlayerLC>();
					zombie.runLogic();
				}
			}

			physics.runPhysics(CLIENT_TIME_STEP);

			combat.runAttackCheck<ServerPlayerLC, ZombieLC>();
			combat.runAttackCheck<ZombieLC, ServerPlayerLC>();
			
			size_t size = ctrls.size();
			ctrls.clear();
			ctrls.reserve(size);

			//handle quitting
			QuitPacket q;
			for (auto& user : users) {
				Connection * con = user->getConnection();
				if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > forceDisconnectDelay) {
					std::cout << "Forcing removal of player " << user->getNetId() << ".\n";
					con->setShouldReset(true);
					q.id = user->getNetId();
					//tell everyone else
					for (auto& other : users) {
						if(other->getNetId() != user->getNetId())
							server.sendPacket<QuitPacket>(other->getNetId(), 0, q);
					}
					server.resetConnection(user->getNetId());
				}
				else if (static_cast<double>(currentTick - con->getLastPacket()) * SERVER_TIME_STEP > disconnectDelay) {
					std::cout << "Attempting to disconnect player " << user->getNetId() << ", no packets received recently.\n";
					server.disconnect(user->getNetId());
				}
			}

			//remove all resets
			auto toReset = [](const UserPtr& user) {
				return user->getConnection()->shouldReset();
			};
			users.erase(std::remove_if(users.begin(), users.end(), toReset), users.end());
		}
	}

	enet_deinitialize();
	SDL_Quit();
	return 0;
}

/*
- create a better sending system, to spread things across multiple channels. packets of each type are required to go allong the same channels
- turn packets into classes. Require serialization and unserialization.
- client side shouldn't set things as truly dead until teh server tells them to. think more about this.

/*
The player state will hold
- Player position
- Player velocity
- time of this state


PlayerLC superclass handles physics
ClientPlayerLC subclass:
- Store the past 32 states
- Re-evaluate the current position based on a change in one of these states.

ServerPlayerLC subclass:
- queue updates
- evaluate queued updates

the client will hold the last 32 player states.
upon recieving a position packet back from the server, it will compare the time of that position packet, to its time.
If its predicted position at a time is different than the servers position, we must recalculate our current position.

A client state packet will have to things:
- Player State
	- Player pos
	- player vel
*/