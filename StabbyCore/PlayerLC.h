#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"
#include "Attack.h"
#include "PhysicsAABB.h"
#include "Stage.h"
#include "PhysicsComponent.h"
#include "PlayerStateComponent.h"

class PlayerLC {
public:
	PlayerLC(EntityId id_ = 0);
	EntityId getId() const;
	void update(double timeDelta, const Controller& controller);
	PhysicsComponent * getPhysics();
	Vec2f getVel() const;
	Vec2f getRes() const;
	Attack& getAttack();
	int getActiveId();
	int getFacing();
	void damage(int amount);
	void die();
	void respawn();

	const static int PLAYER_WIDTH = 4;
	const static int PLAYER_HEIGHT = 20;
protected:
	void free(const Controller & controller, bool attackToggledDown_);
	//as a multiple of acceleration
	EntityId id;
	int maxXVel;
	float xAccel;
	float jumpSpeed;
	//for only getting hit once per slash
	bool isBeingHit;

	Attack attack;
	bool attackBuffered;

	bool prevButton2;
	bool prevButton3;

	float rollVel;
	float storedVel;
	int rollFrameMax;

	float stunSlideSpeed;
	int stunFrameMax;

	int deathFrame;
	int deathFrameMax;
};