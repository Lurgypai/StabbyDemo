#pragma once
#include "ZombieLC.h"
#include <deque>

class ServerZombieLC : public ZombieLC {
public:
	ServerZombieLC(EntityId id_ = 0);

	void runLogic();

	const AABB * getHurtboxes(int * size) const override;
	virtual void die() override;
private:
	std::deque<AABB> prevHurtboxes;
};