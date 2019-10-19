#pragma once
#include "EntitySystem.h"
#include "PlayerLC.h"
#include "PhysicsComponent.h"
#include "Pool.h"
#include "CombatComponent.h"

//defined in CombatComponent.h
class CombatComponent;

class CombatSystem {
public:
	CombatSystem();

	void runAttackCheck(double timeDelta);
	void runAttackCheck(double timeDelta, EntityId id);
private:

	void attackCheck(CombatComponent& attacker, CombatComponent& defender);
	std::vector<EntityId> hitMobs;
	//index for the pool -> the entity who attacked
	//values in the vector -> who was attacked
	Pool<std::vector<EntityId>> hitList{};
	int prevAttackId;
};
