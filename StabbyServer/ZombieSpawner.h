#pragma once
#include "NetworkTypes.h"

class ZombieSpawner {
public:
	ZombieSpawner(Time_t startTime_);
	void trySpawnZombies(Time_t gameTime);

private:
	bool canSpawnZombies;
	int onlineIdIncrementer;
	Time_t startTime;
};