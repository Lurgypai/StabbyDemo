#pragma once
#include "Stage.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"

class PhysicsSystem {
public:
	PhysicsSystem(Stage& stage);

	void runPhysics(double timeDelta);
private:
	Stage & stage;
	std::vector<EntityId> physicsEntities;
};
