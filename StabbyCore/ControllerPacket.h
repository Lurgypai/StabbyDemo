#pragma once
#include "Packet.h"
#define CONT_KEY	"SCT"	//suqua control

//contains state information
class ControllerPacket {
public:
	ControllerPacket() :
		key{ CONT_KEY },
		netId{ 0 },
		time{ 0 },
		state{ 0 }
	{}

	int8_t key[PACKET_KEY_SIZE + 1];

	NetworkId netId;
	//the client side time when the packet was sent
	Time_t time;
	//game time when the packet was sent
	Time_t when;
	enet_uint8 state;

	bool operator!=(const ControllerPacket & other) {
		return netId != other.netId || time != other.time || when != other.when || state != other.state;
	}

	// Inherited via Packet
	void serialize();
	void unserialize();
};