#pragma once
#include "EntitySystem.h"
#include "SpawnSystem.h"

enum RespawnState {
	unselected,
	selecting,
	selected
};

class RespawnComponent {
public:
	RespawnComponent(EntityId id_ = 0);
	EntityId getId() const;

	RespawnState getState() const;
	void loadSpawnList(SpawnSystem& spawns);
	void searchForSpawn();
	EntityId getCurrentSpawn();
	void reset();
private:
	EntityId id;
	EntityId currentSpawnZone;
	std::set<EntityId>::iterator selection;
	std::set<EntityId> spawnList;

	RespawnState state;

	bool prevL;
	bool prevR;
	bool prevSelect;
};