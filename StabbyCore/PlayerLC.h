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

class PlayerLC : public CombatComponent {
public:
	PlayerLC(EntityId id_ = 0);
	void update(double timeDelta, const Controller& controller);
	PhysicsComponent * getPhysics();
	Vec2f getVel() const;
	Vec2f getRes() const;
	Attack& getAttack();
	int getActiveId();
	void respawn();

	void damage(int amount) override;
	void die() override;
	void onAttackLand() override;
	virtual AABB * getActiveHitbox() override;
	int getActiveDamage() override;
	bool readAttackChange() override;
	virtual const AABB * getHurtboxes(int * size) const override;

	const static int PLAYER_WIDTH = 4;
	const static int PLAYER_HEIGHT = 20;
protected:
	void free(const Controller & controller, bool attackToggledDown_);
	//as a multiple of acceleration
	int maxXVel;
	float xAccel;
	float jumpSpeed;
	//for only getting hit once per slash
	bool isBeingHit;

	Attack attack;
	bool attackBuffered;

	bool prevButton2;
	bool prevButton3;
	bool attackChange;

	float rollVel;
	float storedVel;
	int rollFrameMax;

	float stunSlideSpeed;
	int stunFrameMax;
	int attackFreezeFrameMax;

	int deathFrame;
	int deathFrameMax;

	std::vector<AABB> hurtboxes;
	
	State storedState;
};