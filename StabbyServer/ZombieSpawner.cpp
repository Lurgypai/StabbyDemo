#include "pch.h"
#include "ZombieSpawner.h"
#include "ServerClientData.h"
#include "EntitySystem.h"
#include "ServerZombieLC.h"
#include "PhysicsComponent.h"
#include <iostream>

ZombieSpawner::ZombieSpawner(Time_t startTime_) :
	startTime{startTime_},
	onlineIdIncrementer{0},
	canSpawnZombies{true}
{}

void ZombieSpawner::trySpawnZombies(Time_t gameTime) {


	unsigned int now = static_cast<double>(gameTime - startTime) * GAME_TIME_STEP;
	unsigned int nowMinutes = now / 60;
	
	if (now % 10 == 0) {
		if (canSpawnZombies) {
			canSpawnZombies = false;
			std::cout << "Spawned Zombies!\n";
			const int count = 1;
			EntityId id[count];
			EntitySystem::GenEntities(count, &id[0]);
			EntitySystem::MakeComps<ServerZombieLC>(count, &id[0]);
			for (int i = 0; i != count; ++i) {
				EntitySystem::GetComp<PhysicsComponent>(id[i])->teleport({ -400, 0 });
				EntitySystem::GetComp<ServerZombieLC>(id[i])->onlineId = ++onlineIdIncrementer;
			}
		}
	}
	else {
		canSpawnZombies = true;
	}
}
