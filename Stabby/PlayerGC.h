#pragma once
#include "GLRenderer.h"
#include "EntitySystem.h"
#include "AnimatedSprite.h"
#include "PlayerLC.h"
#include "RenderComponent.h"

class PlayerGC : public RenderComponent{
public:
	enum Animation {
		idle,
		walking,
		slash1,
		slash2,
		slash3,
		roll,
		stun,
		dead
	};

	PlayerGC(EntityId id_ = 0);
	void loadAnimations();
	void spawnHead(Vec2f pos);

	void updatePosition() override;
private:
	Vec2f offset;
	bool shouldSpawnHead;
	State prevState;
	float prevXVel;
	int prevAttack;
};