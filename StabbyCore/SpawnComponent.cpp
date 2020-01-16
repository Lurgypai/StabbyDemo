#include "stdafx.h"
#include "SpawnComponent.h"
#include "RandomUtil.h"
#include <PhysicsComponent.h>
#include <PositionComponent.h>

SpawnComponent::SpawnComponent(EntityId id_) :
	id{id_}
{}

const EntityId SpawnComponent::getId() const {
	return id;
}

void SpawnComponent::addSpawnZone(const AABB& zone) {
	spawnZones.push_back(zone);
	spawnPoints.push_back({});
	int pos = spawnPoints.size() - 1;
	for (int i = 0; i != 5; ++i)
		spawnPoints.back().push_back(generateSpawnPos(pos));
}

Vec2f SpawnComponent::generateSpawnPos(int spawnZoneId) {
	AABB spawnZone = spawnZones[spawnZoneId];
	const int maxAttempts = -1;
	for (int i = 0; i != maxAttempts; ++i) {
		Vec2f spawnPos{};
		spawnPos.x = randFloat(spawnZone.pos.x, spawnZone.pos.x + spawnZone.res.x);
		spawnPos.y = randFloat(spawnZone.pos.y, spawnZone.pos.y + spawnZone.res.y);
		float minY = 0;
;		bool spawnable{false};
		for (auto& physics : EntitySystem::GetPool<PhysicsComponent>()) {
			if (physics.collideable) {
				const AABB& collider = physics.getCollider();
				const Vec2f& colliderPos = EntitySystem::GetComp<PositionComponent>(physics.getId())->pos;
				if (spawnPos.x > colliderPos.x && spawnPos.x < colliderPos.x + collider.res.x && spawnPos.y < colliderPos.y) {
					if (!spawnable || colliderPos.y < minY) {
						minY = colliderPos.y;
						spawnable = true;
					}
				}
			}
		}
		if (spawnable)
			return Vec2f{ spawnPos.x, minY };
	}
	return {0, 0};
}

Vec2f SpawnComponent::findSpawnPos() {
	int spawnZone = randInt(0, spawnZones.size() - 1);
	return spawnPoints[spawnZone][randInt(0, spawnPoints[spawnZone].size() - 1)];
}
