#include "stdafx.h"
#include "PhysicsComponent.h"
#include "AngleUtil.h"

PhysicsComponent::PhysicsComponent(EntityId id_, AABB collider_, float weight_, Vec2f vel_) :
	id{ id_ },
	collider{collider_},
	weight{weight_},
	vel{vel_},
	grounded{false}
{}

EntityId PhysicsComponent::getId() const {
	return id;
}

void PhysicsComponent::move(Vec2f amount) {
	collider.pos += amount;
}

void PhysicsComponent::move(float angle, float amount) {
	Vec2f displacement{0, 0};
	displacement.x = std::cos(radians(angle)) * amount;
	displacement.y = std::sin(radians(angle)) * amount;
	collider.pos += displacement;
}

void PhysicsComponent::accelerate(Vec2f amount) {
	vel.x += amount.x;
	vel.y += amount.y;
}

void PhysicsComponent::accelerate(float angle, float amount) {
	Vec2f acceleration{ 0, 0 };
	acceleration.x = std::cos(radians(angle)) * amount;
	acceleration.y = std::sin(radians(angle)) * amount;
	vel += acceleration;
}
