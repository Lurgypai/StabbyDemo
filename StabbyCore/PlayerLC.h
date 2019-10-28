#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"
#include "Attack.h"
#include "PhysicsAABB.h"
#include "Stage.h"
#include "PhysicsComponent.h"
#include "PlayerStateComponent.h"
#include "CombatComponent.h"

class PlayerLC {
public:
	PlayerLC(EntityId id_ = 0);
	void update(double timeDelta, const Controller& controller);
	PhysicsComponent * getPhysics();
	Vec2f getVel() const;
	Vec2f getRes() const;
	void respawn();

	EntityId getId() const;

	const static int PLAYER_WIDTH = 4;
	const static int PLAYER_HEIGHT = 20;
protected:
	EntityId id;

	void free(const Controller & controller, bool attackToggledDown_);
	//as a multiple of acceleration
	float jumpSpeed;
	//for only getting hit once per slash
	bool isBeingHit;
	double stepDistance;
	double climbDistance;
	bool attackBuffered;

	bool prevButton1;
	bool prevButton2;
	bool prevButton3;
	bool attackChange;

	float rollVel;
	float storedVel;
	int rollFrameMax;

	int attackFreezeFrameMax;
	int healFrameMax;
	int healDelayMax;

	int deathFrame;
	int deathFrameMax;

	std::vector<AABB> hurtboxes;
	
	State storedState;
};

/*
set everything with direction to use direction component
complete the implementation of the CombatComponent
add the AttackStats struct

Begin implementation of Effects
*/