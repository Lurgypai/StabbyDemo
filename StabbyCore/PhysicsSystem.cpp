#include "stdafx.h"
#include "PhysicsSystem.h"
#include "PhysicsComponent.h"

PhysicsSystem::PhysicsSystem(Stage & stage_) : stage{stage_} {}

void PhysicsSystem::runPhysics(double timeDelta) {
	for (auto& rawComp : EntitySystem::GetPool<PhysicsComponent>()) {
		PhysicsComponent * comp = &rawComp;
		//accelerate downwards, gravity
		comp->accelerate({ 0, comp->weight });

		comp->grounded = false;

		//handle collisions with the stage
		for (auto & collider : stage.getColliders()) {
			Vec2f & currPos = comp->collider.pos;
			Vec2f & vel = comp->vel;
			Vec2f res = comp->collider.res;
			//place we are updating too
			Vec2f newPos = { currPos.x + vel.x * static_cast<float>(timeDelta), currPos.y + vel.y * static_cast<float>(timeDelta) };

			AABB projection{ newPos, comp->collider.res };

			//handle collisions
			if (collider.intersects(projection)) {
				Vec2f overlap{ 0, 0 };

				//moving down
				if (vel.y > 0) {
					//if we were above
					if (currPos.y + res.y <= collider.pos.y) {
						overlap.y = (newPos.y + res.y) - collider.pos.y;
					}
				}
				//moving up
				else if (vel.y < 0) {
					//below
					if (currPos.y >= collider.pos.y + collider.res.y) {
						overlap.y = newPos.y - (collider.pos.y + collider.res.y);
					}
				}

				//moving right
				if (vel.x > 0) {
					//if we were to the left of it
					if (currPos.x + res.x <= collider.pos.x) {
						overlap.x = (newPos.x + res.x) - collider.pos.x;
					}
				}
				//moving left
				else if (vel.x < 0) {
					if (currPos.x >= collider.pos.x + collider.res.x) {
						overlap.x = newPos.x - (collider.pos.x + collider.res.x);
					}
				}

				//horizontal collision
				if (overlap.x != 0.0f && overlap.y == 0.0f) {
					vel.x = 0.0f;
				}
				//vertical collision
				else if (overlap.x == 0.0f && overlap.y != 0.0f) {
					if (vel.y > 0)
						comp->grounded = true;
					vel.y = 0.0f;
				}
				//corner collision
				else if (overlap.x != 0.0f && overlap.y != 0.0f) {
					float absXVel = abs(vel.x);
					float absYVel = abs(vel.y);
					if (absXVel > absYVel) {
						//this means don't resolve collisions allong the x axis
						overlap.x = 0;
						//and stop moving allong the y axis
						vel.y = 0;
						comp->grounded = true;
					}
					else {
						overlap.y = 0;
						vel.x = 0;
					}
				}
				newPos -= overlap;
			}
			currPos = newPos;
		}
	}
}