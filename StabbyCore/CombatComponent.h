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

	int baseDamage;
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

	void damage(int i);
	void heal(int i);

	//runs damage calculations and returns the result
	int rollDamage();

	Attack attack;
	CombatStats stats;
	int health;
	unsigned int teamId;
	bool invulnerable;
	std::vector<Hurtbox> hurtboxes;
private:
	void updateHurtboxes();

	//effects triggers
	void onAttackLand();
	void onKill();
	void onDeath();
	void onHeal(int amount);
	void onDamage(int amount);

	bool attackChanged;
	EntityId id;
};