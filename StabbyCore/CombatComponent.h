#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class CombatComponent {
public:
	CombatComponent(EntityId id = 0);
	virtual ~CombatComponent() {};
	EntityId getId() const;

	virtual void damage(int i) = 0;
	virtual void die() = 0;
	virtual void onAttackLand() = 0;
	virtual AABB * getActiveHitbox() = 0;
	virtual int getActiveDamage() = 0;
	//checks if the attack changed, and resets it if it did.
	virtual bool readAttackChange() = 0;
	virtual const AABB * getHurtboxes(int * size) const = 0;

protected:
	EntityId id;
};