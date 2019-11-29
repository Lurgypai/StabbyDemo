#include "pch.h"
#include "Game.h"
#include "ClientPlayerLC.h"
#include "PlayerGC.h"
#include "PlayerCam.h"
#include "ZombieGC.h"
#include "ZombieLC.h"
#include "HealthPickupLC.h"
#include "HealthPickupGC.h"

void Game::startOfflineGame() {
	//load up the stage
	//grab the  player spawn point
	//create the player there
	
	Vec2f spawnPos = stage.getSpawnPos();

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<PlayerLC>(1, &playerId);
	EntitySystem::GetComp<CombatComponent>(playerId)->attack = weapons.cloneAttack("player_sword");
	EntitySystem::GetComp<CombatComponent>(playerId)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ 0, 0 }, AABB{ {0, 0}, {4, 20} } });
	EntitySystem::GetComp<CombatComponent>(playerId)->health = 100;
	EntitySystem::GetComp<CombatComponent>(playerId)->stats = CombatStats{ 100, 0, 0, 20, 0.0f, 0.0f, 0.0f, 0.0f };

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadSprite<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	pos->teleport( spawnPos );

	EntityId heartId;
	EntitySystem::GenEntities(1, &heartId);
	EntitySystem::MakeComps<HealthPickupLC>(1, &heartId);
	EntitySystem::MakeComps<HealthPickupGC>(1, &heartId);
	EntitySystem::GetComp<RenderComponent>(heartId)->loadSprite<Sprite>("images/heart.png");
	EntitySystem::GetComp<HealthPickupLC>(heartId)->amount = 20;
	EntitySystem::GetComp<PositionComponent>(heartId)->pos -= {200, 20};
}

void Game::startOnlineGame(const std::string & address, int port) {
	client.connect(address, port);
	
	client.setWeaponManager(weapons);
	Vec2f spawnPos = stage.getSpawnPos();

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<ClientPlayerLC>(1, &playerId);
	EntitySystem::GetComp<ClientPlayerLC>(playerId)->setPhysics(physics);
	EntitySystem::GetComp<ClientPlayerLC>(playerId)->setCombat(combat);
	EntitySystem::GetComp<CombatComponent>(playerId)->attack = weapons.cloneAttack("player_sword");
	EntitySystem::GetComp<CombatComponent>(playerId)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ 0, 0 }, AABB{ {0, 0}, {4, 20} } });
	EntitySystem::GetComp<CombatComponent>(playerId)->health = 100;
	EntitySystem::GetComp<CombatComponent>(playerId)->stats = CombatStats{ 100, 0, 0, 20, 0.0f, 0.0f, 0.0f, 0.0f };

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadSprite<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	pos->teleport(spawnPos);

	client.setPlayer(playerId);
}

EntityId Game::getPlayerId() {
	return playerId;
}
