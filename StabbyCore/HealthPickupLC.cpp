#include "stdafx.h"
#include "HealthPickupLC.h"
#include "PositionComponent.h"
#include "AngleUtil.h"
#include "EntityBaseComponent.h"

HealthPickupLC::HealthPickupLC(EntityId id_) :
	id{ id_ },
	collider{ {0, 0}, { 9, 9 }}
{
	if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PositionComponent>(1, &id);
	}
}

EntityId HealthPickupLC::getId() const {
	return id;
}

void HealthPickupLC::update(time_t clientTime, double timeDelta) {
	float yOffset = sin(radians(static_cast<double>(clientTime) * 1.3)) * 3;
	EntitySystem::GetComp<PositionComponent>(id)->pos.y += yOffset * timeDelta;
}

void HealthPickupLC::updateColliders() {
	collider.pos = EntitySystem::GetComp<PositionComponent>(id)->pos;
}

void HealthPickupLC::die() {
	EntitySystem::GetComp<EntityBaseComponent>(id)->isDead = true;
}

AABB * HealthPickupLC::getColliders(int * size) {
	if (size != nullptr)
		*size = 1;
	return &collider;
}
