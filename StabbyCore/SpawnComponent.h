#pragma once
#include "EntitySystem.h"
#include <vector>
#include <AABB.h>

class SpawnComponent {
public:
	SpawnComponent(EntityId id_ = 0);
	const EntityId getId() const;

	void addSpawnZone(const AABB& zone);
	Vec2f generateSpawnPos(int spawnZoneId);
	Vec2f findSpawnPos();

private:
	EntityId id;
	std::vector<AABB> spawnZones;
	std::vector<std::vector<Vec2f>> spawnPoints;
};