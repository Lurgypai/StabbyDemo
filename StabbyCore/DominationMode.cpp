#include "stdafx.h"
#include "DominationMode.h"
#include "SpawnComponent.h"
#include "RandomUtil.h"
#include "PositionComponent.h"
#include "CapturePointComponent.h"

void DominationMode::load(SpawnSystem* spawns, unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_)
{
	loadData(totalTeams_, pointsPerCap_, winningPoints_);
	assignPlayers();
	assignSpawns(spawns);
}

void DominationMode::loadData(unsigned int totalTeams_, unsigned int pointsPerCap_, unsigned int winningPoints_) {
	totalTeams = totalTeams_;
	pointsPerCap = pointsPerCap_;
	winningPoints = winningPoints_;
}

void DominationMode::assignPlayers() {
	std::vector<EntityId> players;
	if (EntitySystem::Contains<PlayerLC>()) {
		size_t size = EntitySystem::GetPool<PlayerLC>().size();
		players.reserve(size);
		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			players.emplace_back(player.getId());
		}
	}

	while (!players.empty()) {
		size_t pos = randInt(0, players.size() - 1);
		EntityId playerId = players[pos];
		addPlayer(playerId);
		players.erase(players.begin() + pos);
	}
}

void DominationMode::assignSpawns(SpawnSystem* spawns) {
	std::vector<unsigned int> teamIds(totalTeams, 0);
	for (unsigned int i = 1; i != totalTeams + 1; teamIds[i - 1] = i, ++i);
	for (auto& id : teamIds)
		teams[id].teamId = id;
	auto teamIdCpy = teamIds;

	//get all spawn points unassigned, assign them
	auto defaultSpawns = spawns->getSpawnPoints(0);
	for (auto& spawnId : defaultSpawns) {
		SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(spawnId);
		if (spawn->isDefault()) {
			auto pos = randInt(0, teamIdCpy.size() - 1);
			auto teamId = teamIdCpy[pos];
			teamIdCpy.erase(teamIdCpy.begin() + pos);
			spawns->assignTeam(spawnId, teamId);

			EntityId capturePoint = spawn->getId();
			createZone(capturePoint, spawn->getSpawnZone(), teamId, 13 * 120, 13 * 120);

			if (teamIdCpy.empty())
				teamIdCpy = teamIds;
		}
	}
}

void DominationMode::createZone(EntityId capturePoint, AABB zone, uint64_t teamId, float totalCaptureTime_, float remainingCaptureTime) {
	EntitySystem::MakeComps<CapturePointComponent>(1, &capturePoint);
	CapturePointComponent* capture = EntitySystem::GetComp<CapturePointComponent>(capturePoint);
	capture->zone = zone;
	capture->currTeamId = teamId;
	capture->targetTeamid = teamId;
	capture->totalCaptureTime = totalCaptureTime_;
	capture->remainingCaptureTime = remainingCaptureTime;
}

void DominationMode::tickCapturePoints(SpawnSystem& spawns, double timeDelta) {
	if (EntitySystem::Contains<CapturePointComponent>()) {
		for (auto& capturePoint : EntitySystem::GetPool<CapturePointComponent>()) {
			bool wasCapped = capturePoint.isCaptured();
			capturePoint.tickCapturePoint(timeDelta);
			if (capturePoint.isCaptured()) {
				teams[capturePoint.currTeamId].points += pointsPerCap;
				//if we weren't capped, but now we are
				if (!wasCapped) {
					//propogate change to spawn
					spawns.assignTeam(capturePoint.getId(), capturePoint.currTeamId);
				}
			}
		}
	}
	//tick all the points
	//increment current team points
	//if any changed, propogate that change to the spawn
}

void DominationMode::addPlayer(EntityId id) {
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);
	unsigned int targetTeamId = 1;
	int minSize = -1;
	for (auto& pair : teams) {
		if (pair.second.players.size() < minSize || minSize == -1) {
			targetTeamId = pair.second.teamId;
			minSize = pair.second.players.size();
		}
	}

	combat->teamId = targetTeamId;
	teams[targetTeamId].players.push_back(id);
}

void DominationMode::removePlayer(EntityId id) {
	for (auto& pair : teams) {
		for (auto iter = pair.second.players.begin(); iter != pair.second.players.end(); ++iter) {
			if (*iter == id) {
				pair.second.players.erase(iter);
				return;
			}
		}
	}
}

/*
assuming there are 5 capture points on a map
you get 120 * pointsPerCap = 120 points a second
times 5 points = 120 * 5 = 600 points per second
times 60 seconds times 4 minutes = 600 * 60 * 4 = 144000 points 
*/