#pragma once
#include "EntitySystem.h"
#include "PlayerLC.h"
#include "PhysicsComponent.h"
#include "Pool.h"

class CombatSystem {
public:
	CombatSystem();

	void runAttackCheck(double timeDelta);
private:
	std::vector<EntityId> hitMobs;
	//index for the pool -> the entity who attacked
	//values in the vector -> who was attacked
	Pool<std::vector<EntityId>> hitList{};
	int prevAttackId;
};
