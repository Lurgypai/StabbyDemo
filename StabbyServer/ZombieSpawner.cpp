#include "pch.h"
#include "ZombieSpawner.h"
#include "ServerClientData.h"
#include "EntitySystem.h"
#include "PhysicsComponent.h"
#include "ZombieLC.h"
#include "RandomUtil.h"
#include <iostream>

#define MAX_ZOMBIES 20

ZombieSpawner::ZombieSpawner(Time_t startTime_) :
	startTime{startTime_},
	onlineIdIncrementer{0},
	canSpawnZombies{true}
{}

void ZombieSpawner::trySpawnZombies(Time_t gameTime) {

	unsigned int now = static_cast<double>(gameTime - startTime) * GAME_TIME_STEP;
	unsigned int nowMinutes = now / 60;
	
	float side = 0.0f;
	if (randInt(0, 1))
		side = 1.0f;
	else
		side = -1.0f;

	if (now % 15 == 0) {
		if (canSpawnZombies) {
			canSpawnZombies = false;
			std::cout << "Spawned Zombies!\n";
			int count = randInt(1, 7);
			//int count = MAX_ZOMBIES;
			EntityId id[MAX_ZOMBIES];
			EntitySystem::GenEntities(count, &id[0]);
			EntitySystem::MakeComps<ZombieLC>(count, &id[0]);
			for (int i = 0; i != count; ++i) {
				EntitySystem::GetComp<PhysicsComponent>(id[i])->teleport({ 350.0f * side + randInt(-20, 20), 0 });
				EntitySystem::GetComp<ZombieLC>(id[i])->onlineId = ++onlineIdIncrementer;
			}
		}
	}
	else {
		canSpawnZombies = true;
	}
}
