#include "stdafx.h"
#include "SpawnSystem.h"

EntityId SpawnSystem::addSpawnZone(const AABB& zone, bool defaultZone_, unsigned int teamId) {
	EntityId id;
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<SpawnComponent>(1, &id);
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(id);
	spawn->spawnZone = zone;
	spawn->defaultSpawnZone = defaultZone_;
	spawn->owningTeam = teamId;
	spawn->generateSpawns();
	teamSpawns[teamId].insert(id);
	return id;
}

const std::set<EntityId>& SpawnSystem::getSpawnPoints(unsigned int teamId) const {
	auto spawn = teamSpawns.find(teamId);
	if (spawn != teamSpawns.end()) {
		return spawn->second;
	}
	else {
		throw std::exception();
		//throw an exception, specialize this later
	}
}

bool SpawnSystem::teamHasSpawns(unsigned int teamId) const {
	return teamSpawns.find(teamId) != teamSpawns.end();
}

void SpawnSystem::assignTeam(EntityId spawnZone, unsigned int teamId) {
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(spawnZone);
	auto oldTeam = spawn->owningTeam;
	spawn->owningTeam = teamId;
	teamSpawns[oldTeam].erase(teamSpawns[oldTeam].find(spawnZone));
	teamSpawns[teamId].insert(spawnZone);
}

void SpawnSystem::clearTeamsAssigns() {
	teamSpawns.clear();
	for (auto& spawn : EntitySystem::GetPool<SpawnComponent>()) {
		spawn.owningTeam = 0;
		teamSpawns[0].insert(spawn.getId());
	}
}
