#include "pch.h"
#include "Game.h"
#include "PlayerGC.h"
#include "PlayerCam.h"
#include "ZombieGC.h"
#include "ZombieLC.h"
#include "HealthPickupLC.h"
#include "HealthPickupGC.h"
#include <EntityBaseComponent.h>
#include <SpawnComponent.h>
#include <ClientPlayerComponent.h>

Game::Game() :
	physics{},
	combat{},
	clientPlayers{&physics, &combat}
{}

void Game::startOfflineGame(const std::string & stageName) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}

	editables.isEnabled = false;
	//load up the stage
	//grab the  player spawn point
	//create the player there
	
	loadStage(stageName);

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<PlayerLC>(1, &playerId);
	EntitySystem::GetComp<CombatComponent>(playerId)->attack = weapons.cloneAttack("player_sword");
	EntitySystem::GetComp<CombatComponent>(playerId)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ 0, 0 }, AABB{ {0, 0}, {4, 20} } });
	EntitySystem::GetComp<CombatComponent>(playerId)->health = 100;
	EntitySystem::GetComp<CombatComponent>(playerId)->stats = CombatStats{ 100, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(stage.getSpawnable());
	pos->teleport(spawn->findSpawnPos());
}

void Game::startOnlineGame(const std::string & address, int port, const std::string & stageName) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	editables.isEnabled = false;

	loadStage(stageName);

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<PlayerLC>(1, &playerId);
	EntitySystem::MakeComps<ClientPlayerComponent>(1, &playerId);
	EntitySystem::GetComp<CombatComponent>(playerId)->attack = weapons.cloneAttack("player_sword");
	EntitySystem::GetComp<CombatComponent>(playerId)->hurtboxes.emplace_back(Hurtbox{ Vec2f{ 0, 0 }, AABB{ {0, 0}, {4, 20} } });
	EntitySystem::GetComp<CombatComponent>(playerId)->health = 100;
	EntitySystem::GetComp<CombatComponent>(playerId)->stats = CombatStats{ 100, 0, 0, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f };

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	SpawnComponent* spawn = EntitySystem::GetComp<SpawnComponent>(stage.getSpawnable());
	pos->teleport(spawn->findSpawnPos());

	client.setPlayer(playerId);

	client.connect(address, port);

	client.setWeaponManager(weapons);
	client.setClientPlayerSystem(&clientPlayers);
}

void Game::startStageEditor(const std::string & filePath) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}


	editables.isEnabled = true;
	editables.load(filePath);
}

void Game::loadStage(const std::string& stageName) {
	stage = Stage(stageName);
	stage.loadGraphics();
}

EntityId Game::getPlayerId() {
	return playerId;
}

const Stage& Game::getStage() const {
	return stage;
}
