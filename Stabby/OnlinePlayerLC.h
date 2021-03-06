#pragma once
#include "NetworkTypes.h"
#include "EntitySystem.h"
#include "Vec2.h"
#include "PlayerLC.h"
class OnlinePlayerLC {
public:
	OnlinePlayerLC(EntityId id_ = 0);
	EntityId getId() const;
	//add a position to move towards
	void interp(PlayerState st, Time_t when);
	void update(Time_t gameTime);
private:
	//the current and two previous positions. We are moving between the two previous positions.
	Vec2f previousPos[3];
	//when the current and two previous positions arrived.
	Time_t whens[3];
	EntityId id;
};