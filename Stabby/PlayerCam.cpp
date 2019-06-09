#include "pch.h"
#include "PlayerCam.h"
#include "ClientPlayerLC.h"
#include "PositionComponent.h"
#include <iostream>

PlayerCam::PlayerCam(int windowWidth, int windowHeight) :
	Camera{ Vec2i{0, 0}, Vec2i{windowWidth, windowHeight}, 1 }
{
}

void PlayerCam::update(EntityId targetId) {

	Vec2f prevPos = pos;

	if (EntitySystem::Contains<PhysicsComponent>()) {
		PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(targetId);
		if (physics != nullptr) {
			center(physics->center());
		}
		else {
			PositionComponent * position = EntitySystem::GetComp<PositionComponent>(targetId);
			if (position != nullptr) {
				center(position->pos);
			}
		}
	}

	Vec2f newPos = pos;

	//bigger number here means move slower
	pos = prevPos + ((newPos - prevPos) / 3);
}
