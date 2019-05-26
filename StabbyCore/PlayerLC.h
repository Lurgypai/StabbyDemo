#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "Controller.h"
#include "Attack.h"
#include "PhysicsAABB.h"
#include "Stage.h"
#include "PhysicsComponent.h"

enum class State {
	free,
	attacking,
	rolling,
	stunned,
	dead
};

class PlayerLC {
public:
	PlayerLC(EntityId id_ = 0);
	EntityId getId() const;
	void update(double timeDelta, const Controller& controller);
	PhysicsComponent * getPhysics();
	Vec2f getVel() const;
	Vec2f getRes() const;
	Attack& getAttack();
	State getState();
	int getActiveId();
	int getRollFrame();
	int getStunFrame();
	int getFacing();
	int getHealth();
	void damage(int amount);
	void die();
	void kill();
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

	State state;

	Attack attack;
	bool attackBuffered;

	int facing;
	bool prevButton2;
	bool prevButton3;
	int health;

	float rollVel;
	float storedVel;
	int rollFrame;
	int rollFrameMax;

	float stunSlideSpeed;
	int stunFrame;
	int stunFrameMax;

	int deathFrame;
	int deathFrameMax;
};