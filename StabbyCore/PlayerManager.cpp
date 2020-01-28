#include "stdafx.h"
#include "PlayerManager.h"
#include <PlayerLC.h>
#include <WeaponManager.h>
#include <SpawnComponent.h>
#include <DebugIO.h>

EntityId PlayerManager::makePlayer(const WeaponManager & weapons) {
	EntityId playerId;
	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<PlayerLC>(1, &playerId);
	EntitySystem::GetComp<CombatComponent>(playerId)->attack = weapons.cloneAttack("player_sword");
	EntitySystem::GetComp<CombatComponent>(playerId)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ 0, 0 }, AABB{ {0, 0}, {4, 20} } });
	EntitySystem::GetComp<CombatComponent>(playerId)->health = 100;
	EntitySystem::GetComp<CombatComponent>(playerId)->stats = CombatStats{ 100, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	return playerId;
}

void PlayerManager::updateAll(double timeDelta, const Controller & cont, const Stage & stage) {
	if (EntitySystem::Contains<PlayerLC>()) {
		for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
			player.update(timeDelta, cont);

			if (player.shouldRespawn()) {
				SpawnComponent * spawnZones = EntitySystem::GetComp<SpawnComponent>(stage.getSpawnable());
				player.respawn(spawnZones->findSpawnPos());
			}
		}
	}
}

void PlayerManager::update(EntityId playerId, double timeDelta, const Controller& cont, const Stage& stage) {
	if (EntitySystem::Contains<PlayerLC>()) {
		PlayerLC* player = EntitySystem::GetComp<PlayerLC>(playerId);
		player->update(timeDelta, cont);
		if (player->shouldRespawn()) {
			SpawnComponent* spawnZones = EntitySystem::GetComp<SpawnComponent>(stage.getSpawnable());
			player->respawn(spawnZones->findSpawnPos());
		}

	}
}