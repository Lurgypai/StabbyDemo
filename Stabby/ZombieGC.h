#pragma once
#include "ZombieLC.h"
#include "RenderComponent.h"

class ZombieGC : public RenderComponent {
public:

	ZombieGC(EntityId id_ = 0);
	void loadAnimations();
	void updatePosition() override;

private:
	Vec2f offset;
	Vec2f center;
	int facing;
	float prevXVel;
	int deathFrame;
	int deathFrameMax;
};