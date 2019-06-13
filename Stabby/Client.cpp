#include "pch.h"
#include <numeric>
#include "NetworkTypes.h"

#include "DebugIO.h"
#include "Client.h"
#include "EntitySystem.h"
#include "OnlinePlayerLC.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "ServerClientData.h"
#include "ZombieGC.h"
#include <iostream>

Client::Client() :
	id{ 0 }
{
	enet_initialize();
	DebugIO::printLine("Starting client.");
	if (!client.createClient(1, 3)) {
		DebugIO::printLine("Error while trying to create client.");
		return;
	}
	connected = false;
}

Client::~Client() {
	enet_deinitialize();
}

void Client::connect(Time_t now, const std::string & ip, int port) {
	if (!connected) {
		serverId = client.connect(ip, port, 3);

		ENetEvent e;
		if (client.service(&e, 5000) > 0 && e.type == ENET_EVENT_TYPE_CONNECT) {
			DebugIO::printLine("Connection established.");
		}

		//attempt to get the time, as well as the networkid.

		bool handshakeComplete{ false };
		while (client.service(&e, 5000) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				client.removePeer(serverId);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				//this portion will denote a welcome packet
				if (PacketUtil::readPacketKey(e.packet) == WELCOME_KEY) {
					WelcomePacket packet{};
					std::memcpy(&packet, e.packet->data, e.packet->dataLength);
					packet.unserialize();
					DebugIO::printLine("Handshake Complete!");
					DebugIO::printLine("Time is " + std::to_string(packet.currentTick) + ", and our ID is " + std::to_string(packet.netId) + "!");
					networkTime = packet.currentTick;
					id = packet.netId;
					handshakeComplete = true;
				}
				break;
			}
			if (handshakeComplete) {
				break;
			}
		}
		if (!handshakeComplete) {
			DebugIO::printLine("Unable to complete handshake, disconnecting.");
			client.disconnect(serverId);
		}
		else {
			TimestampPacket p;
			p.clientTime = now;
			p.id = id;
			client.sendPacket(serverId, 0, p);
			connected = true;
		}
	}
	else {
		DebugIO::printLine("We are already connected to a server.");
	}
}

void Client::service(Time_t now_) {
	now = now_;
	if (connected) {
		ENetEvent e;
		while (client.service(&e, 0) > 0) {
			switch (e.type) {
			case ENET_EVENT_TYPE_DISCONNECT:
				DebugIO::printLine("Disconnected from server.");
				connected = false;
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				receive(e);
				break;
			}
		}
	}
}



void Client::ping(Time_t now) {
	TimestampPacket p;
	p.id = id;
	p.clientTime = now;
	client.sendPacket(serverId, 0, p);
}

void Client::recalculatePing(Time_t nextPing) {
	if (pings.size() >= PING_COUNT)
		pings.pop_front();
	pings.push_back(nextPing);

	currentPing = (std::accumulate(pings.begin(), pings.end(), 0)) / pings.size();
}

unsigned int Client::getPing() {
	return currentPing * CLIENT_TIME_STEP * 100;
}

bool Client::getConnected() {
	return connected;
}

Time_t Client::getTime() {
	return networkTime;
}

void Client::progressTime(Time_t delta) {
	networkTime += delta;
}

NetworkId Client::getNetId() {
	return id;
}

void Client::setPlayer(EntityId id_) {
	playerId = id_;
}

bool Client::isBehindServer() {
	return behindServer;
}

void Client::resetBehindServer() {
	behindServer = false;
}

void Client::receive(ENetEvent & e) {
	std::string packetKey = PacketUtil::readPacketKey(e.packet);
	std::vector<NetworkId> ids;
	if (packetKey == JOIN_KEY) {
		JoinPacket p;
		PacketUtil::readInto<JoinPacket>(p, e.packet);
		p.unserialize();
		DebugIO::printLine("Player with id " + std::to_string(p.joinerId) + " has joined!");
		ids.push_back(p.joinerId);
	}
	else if (packetKey == STATE_KEY) {
		std::vector<StatePacket> states;
		size_t size = e.packet->dataLength / sizeof(StatePacket);
		states.resize(size);

		PacketUtil::readInto<StatePacket>(&states[0], e.packet, size);


		for (auto & p : states) {
			p.unserialize();
			if (EntitySystem::Contains<OnlinePlayerLC>()) {
				Pool<OnlinePlayerLC> & onlinePlayers = EntitySystem::GetPool<OnlinePlayerLC>();
				for (auto& onlinePlayer : onlinePlayers) {
					if (onlinePlayer.getNetId() == p.id) {
						onlinePlayer.interp(p.state, p.when);
					}
				}
			}
			if (EntitySystem::Contains<ClientPlayerLC>()) {
				if (id == p.id) {
					ClientPlayerLC * player = EntitySystem::GetComp<ClientPlayerLC>(playerId);
					if (player != nullptr) {
						player->repredict(p.state);
						//if we're behind where the server is;
						if(!behindServer)
							behindServer = p.state.when > now;
					}
				}
			}
		}
	}
	else if (packetKey == ZOMBIE_KEY) {
		std::vector<ZombiePacket> states;
		size_t size = (e.packet->dataLength - sizeof(Time_t)) / sizeof(ZombiePacket);
		states.resize(size);

		PacketUtil::readInto<ZombiePacket>(&states[0], e.packet, size);
		Time_t when;
		memcpy(&when, e.packet->data + sizeof(ZombiePacket) * size, sizeof(Time_t));
		when = ntohll(when);

		if(!behindServer)
			behindServer = when > now;

		//to be removed, in favor of a storing the "master sprite" inside the zombiegc
		static AnimatedSprite sprite{ "images/zombie.png", Vec2i{32, 32} };

		for (auto& zombieState : states) {
			zombieState.unserialize();
			if (EntitySystem::Contains<ZombieLC>()) {
				if (idTable.contains(zombieState.onlineId)) {
					//remove this in favor of the server determining when a zombie dies.
					ZombieLC * zombie = EntitySystem::GetComp<ZombieLC>(idTable[zombieState.onlineId]);
					if(zombie != nullptr)
						zombie->setState(zombieState.state);
				}
				else {
					EntityId id;
					EntitySystem::GenEntities(1, &id);
					EntitySystem::MakeComps<ZombieLC>(1, &id);
					EntitySystem::MakeComps<ZombieGC>(1, &id);
					EntitySystem::GetComp<ZombieLC>(id)->onlineId = zombieState.onlineId;
					EntitySystem::GetComp<ZombieLC>(id)->setState(zombieState.state);
					EntitySystem::GetComp<ZombieGC>(id)->setSprite<AnimatedSprite>(sprite);
					EntitySystem::GetComp<ZombieGC>(id)->loadAnimations();
					idTable.add(zombieState.onlineId, id);
				}
			}
			else {
				EntityId id;
				EntitySystem::GenEntities(1, &id);
				EntitySystem::MakeComps<ZombieLC>(1, &id);
				EntitySystem::MakeComps<ZombieGC>(1, &id);
				EntitySystem::GetComp<ZombieLC>(id)->onlineId = zombieState.onlineId;
				EntitySystem::GetComp<ZombieLC>(id)->setState(zombieState.state);
				EntitySystem::GetComp<ZombieGC>(id)->setSprite<AnimatedSprite>(sprite);
				EntitySystem::GetComp<ZombieGC>(id)->loadAnimations();
				idTable.add(zombieState.onlineId, id);
			}
		}
	}
	else if (packetKey == QUIT_KEY) {
		QuitPacket q;
		PacketUtil::readInto<QuitPacket>(q, e.packet);
		q.unserialize();
		for (auto iter = EntitySystem::GetPool<OnlinePlayerLC>().beginResource(); iter != EntitySystem::GetPool<OnlinePlayerLC>().endResource(); iter++) {
			auto& onlinePlayer = *iter;
			if (!onlinePlayer.isFree && onlinePlayer.val.getNetId() == q.id) {
				//remove
				onlinePlayer.isFree = true;
				EntityId id = onlinePlayer.val.getId();
				EntitySystem::FreeComps<PlayerGC>(1, &id);
			}
		}
		DebugIO::printLine("Player " + std::to_string(q.id) + " has disconnected.");
	}
	else if (packetKey == TIME_KEY) {
		TimestampPacket p;
		PacketUtil::readInto<TimestampPacket>(p, e.packet);
		p.unserialize();
		Time_t latestRtt = now - p.clientTime;
		recalculatePing(latestRtt);
		//kinda maybe synchronized
		networkTime = p.gameTime + (((static_cast<double>(currentPing) / 2) * CLIENT_TIME_STEP) / GAME_TIME_STEP);
	}

	if (!ids.empty()) {
		std::vector<EntityId> entities;
		entities.resize(ids.size());
		EntitySystem::GenEntities(ids.size(), &entities[0]);
		EntitySystem::MakeComps<OnlinePlayerLC>(entities.size(), &entities[0]);
		EntitySystem::MakeComps<PlayerGC>(entities.size(), &entities[0]);

		for (int i = 0; i != ids.size(); ++i) {
			EntityId entity = entities[i];
			NetworkId netId = ids[i];
			EntitySystem::GetComp<OnlinePlayerLC>(entity)->setNetId(netId);
			EntitySystem::GetComp<PlayerGC>(entity)->loadSprite<AnimatedSprite>("images/stabbyman_with_hilt.png", Vec2i{ 64, 64 });
			EntitySystem::GetComp<PlayerGC>(entity)->loadAnimations();
		}
	}
}
