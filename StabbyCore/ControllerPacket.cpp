#include "stdafx.h"
#include "ControllerPacket.h"

void ControllerPacket::serialize() {
	netId = htonll(netId);
	time = htonll(time);
	when = htonll(when);
}

void ControllerPacket::unserialize() {
	netId = ntohll(netId);
	time = ntohll(time);
	when = ntohll(when);
}
