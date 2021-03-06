#pragma once
#include "ZombieLC.h"
#include "RenderComponent.h"

class ZombieGC {
public:

	ZombieGC(EntityId id_ = 0);
	void loadAnimations();
	void updateState(double timeDelta);

	EntityId getId() const;
private:
	EntityId id;

	Vec2f center;
	float prevXVel;
};