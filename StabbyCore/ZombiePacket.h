#pragma once
#include "Packet.h"
#include "ZombieLC.h"
#define ZOMBIE_KEY	"SZM"	//temporary zombie state sender

class ZombiePacket {
public:
	ZombiePacket() :
		key{ ZOMBIE_KEY }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	ZombieLC::ZombieState state;
	enet_uint32 onlineId;
	//client side time when this update would have occured (unique per client)
	Time_t when;

	//kill the zombie client side
	bool isDead;

	//used for storing floats over the network
	Vec2i pos;
	Vec2i vel;

	// Inherited via Packet
	void serialize();
	void unserialize();
};