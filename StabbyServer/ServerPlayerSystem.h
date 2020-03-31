#pragma once
#include "NetworkTypes.h"
#include "Stage.h"
#include "PlayerManager.h"

class ServerPlayerSystem {
public:
	void updatePlayers(PlayerManager & players, Time_t gameTime, const Stage & stage, SpawnSystem& spawns);
	void tickPlayerTimes();
};