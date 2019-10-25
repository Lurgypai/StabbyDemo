#pragma once
#include <string>
#include "enet/enet.h"
#include "PacketTypes.h"
#include "EntitySystem.h"
#include "Stage.h"
#include "Host.h"
#include "WeaponManager.h"
#include <deque>

#define PING_COUNT 10

class Client {
public:
	Client();

	~Client();

	void connect(Time_t now, const std::string & ip, int port);

	template<typename Packet>
	void send(Packet p);

	void send(size_t size, void* data);

	void service(Time_t now_);

	void ping(Time_t now);
	void recalculatePing(Time_t nextPing);

	//returns ping in milliseconds
	unsigned int getPing();

	bool getConnected();

	Time_t getTime();
	void progressTime(Time_t delta);

	NetworkId getNetId();

	void setPlayer(EntityId id_);
	bool isBehindServer();
	//once we've told the server the time, and what we're doing, this is done to reset that we are behind.
	void resetBehindServer();

	void setWeaponManager(WeaponManager& weapons_);
private:
	void receive(ENetEvent & e);

	
	//client time
	Time_t now;
	//the last PING_COUT pings.
	std::deque<Time_t> pings;
	//average ping in local client time
	Time_t currentPing;

	NetworkId serverId;
	bool connected;
	NetworkId id;
	EntityId playerId;
	//gameTime
	Time_t networkTime;
	//if we're behind the server, so that we can tell the server the current time.
	bool behindServer;
	Pool<EntityId> idTable;
	Host client;
	WeaponManager* weapons;
};

template<typename Packet>
inline void Client::send(Packet p) {
	client.sendPacket<Packet>(serverId, 0, p);
}
