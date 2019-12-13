#pragma once
#include "EntitySystem.h"
#include "CombatSystem.h"
#include "AABB.h"
#include "Attack.h"

struct Hurtbox {
	//offset from the middle bottom of the entity being described
	Vec2f offset;
	AABB box;
};

struct CombatStats {
	int maxHealth;
	int regeneration;
	int shields;

	float baseDamage;
	float critChance;
	float critMultiplier;
	float vampirismChance;
	float vampirismMultiplier;
};

class CombatComponent {

friend class CombatSystem;

public:
	CombatComponent(EntityId id = 0);
	virtual ~CombatComponent() {};
	EntityId getId() const;

	AABB getBoundingBox();
	const Hitbox * getActiveHitbox() const;
	unsigned int getStun();
	bool isStunned();

	void damage(unsigned int i);
	void heal(unsigned int i);
	void stun(unsigned int i);

	//runs damage calculations and returns the result
	int rollDamage();
	EntityId getLastAttacker();

	Attack attack;
	CombatStats stats;
	int health;
	unsigned int teamId;
	unsigned int stunFrame;
	bool invulnerable;
	std::vector<Hurtbox> hurtboxes;
private:
	void updateStun();
	void updateHurtboxes();

	//effects triggers
	void onAttackLand();
	void onKill();
	void onDeath();
	void onHeal(unsigned int amount);
	void onDamage(unsigned int amount);
	void onStun(unsigned int amount);

	bool attackChanged;
	EntityId id;
	EntityId lastAttacker;
};