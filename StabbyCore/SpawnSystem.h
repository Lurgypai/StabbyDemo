#pragma once
#include "SpawnComponent.h"
#include <unordered_map>
#include <set>

class SpawnSystem {
public:
	EntityId addSpawnZone(const AABB& zone, bool defaultZone_, unsigned int teamId);
	const std::set<EntityId>& getSpawnPoints(unsigned int teamId) const;
	bool teamHasSpawns(unsigned int teamId) const;
	void assignTeam(EntityId spawnZone, unsigned int teamId);
	//remove the assignment to teams
	void clearTeamsAssigns();
private:
	//key: teamId, value: spawn
	std::unordered_map<unsigned int, std::set<EntityId> > teamSpawns;
};