#include "stdafx.h"
#include "ZombiePacket.h"

void ZombiePacket::serialize() {
	state.facing = htonl(state.facing);
	state.health = htonl(state.health);
	state.relaxFrame = htonl(state.relaxFrame);
	state.stunFrame = htonl(state.stunFrame);
	state.chargeFrame = htonl(state.chargeFrame);
	state.attackFrame = htonl(state.attackFrame);
	state.deathFrame = htonl(state.deathFrame);
	state.when = htonll(state.when);

	pos.x = htonf(state.pos.x);
	pos.y = htonf(state.pos.y);
	vel.x = htonf(state.vel.x);
	vel.y = htonf(state.vel.y);

	onlineId = htonl(onlineId);
	when = htonll(when);
}

void ZombiePacket::unserialize() {
	state.facing = ntohl(state.facing);
	state.health = ntohl(state.health);
	state.relaxFrame = ntohl(state.relaxFrame);
	state.stunFrame = ntohl(state.stunFrame);
	state.chargeFrame = ntohl(state.chargeFrame);
	state.attackFrame = ntohl(state.attackFrame);
	state.deathFrame = ntohl(state.deathFrame);
	state.when = htonll(state.when);

	state.pos.x = ntohf(pos.x);
	state.pos.y = ntohf(pos.y);
	state.vel.x = ntohf(vel.x);
	state.vel.y = ntohf(vel.y);

	onlineId = ntohl(onlineId);
	when = ntohll(when);
}
