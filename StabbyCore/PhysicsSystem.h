#pragma once
#include "Stage.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"

class PhysicsSystem {
public:
	PhysicsSystem();
	
	void runPhysics(double timeDelta);
	void runPhysics(double timeDelta, EntityId entity);
};