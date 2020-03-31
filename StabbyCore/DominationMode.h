#pragma once
#include <vector>
#include "Stage.h"
#include "CombatComponent.h"
#include <unordered_map>

struct Team {
	std::vector<EntityId> players;
	unsigned int teamId;
	unsigned int points;
};

class DominationMode {
public:
	void load(SpawnSystem* spawns, unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_);
	void loadData(unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_);
	void assignPlayers();
	void assignSpawns(SpawnSystem* spawns);
	void createZone(EntityId capturePoint, AABB zone, uint64_t teamId, float totalCaptureTime_, float remainingCaptureTime);
	void tickCapturePoints(SpawnSystem& spawns, double timeDelta);
	void addPlayer(EntityId id);
	void removePlayer(EntityId id);
private:
	std::unordered_map<unsigned int, Team> teams;
	unsigned int winningPoints;
	unsigned int pointsPerCap;
	unsigned int totalTeams;
};