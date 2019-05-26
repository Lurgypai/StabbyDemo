#pragma once
#include "AABB.h"

class PhysicsComponent {
public:
	PhysicsComponent(EntityId id_ = 0, AABB collider = AABB{ {0, 0}, {0, 0} }, float weight = 0, Vec2f vel = {0, 0});
	EntityId getId() const;

	void move(Vec2f amount);
	void move(float angle, float amount);
	void accelerate(Vec2f amount);
	void accelerate(float angle, float amount);

	float weight;
	Vec2f vel;
	AABB collider;
	bool grounded;
protected:
	EntityId id;
};