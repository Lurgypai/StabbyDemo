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
#include <ControllerComponent.h>
#include <RectDrawable.h>
#include "CapturePointGC.h"
#include <fstream>

Game::Game() :
	physics{},
	combat{},
	clientPlayers{&physics, &combat}
{}

void Game::startOfflineGame() {

	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}

	editables.isEnabled = false;
	//load up the stage
	//grab the  player spawn point
	//create the player there
	
	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.load(&spawns, 2, 1, 6000);

	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());
}

void Game::startOnlineGame() {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}
	editables.isEnabled = false;

	std::ifstream settingsFile{ "settings.json" };
	json settings{};
	settingsFile >> settings;
	std::string stageName = settings["stage"];
	std::string address = settings["ip"];
	int port = settings["port"];

	loadStage(stageName);

	playerId = players.makePlayer(weapons);

	EntitySystem::MakeComps<ClientPlayerComponent>(1, &playerId);

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<RenderComponent>(playerId)->loadDrawable<AnimatedSprite>("images/stabbyman.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();
	EntitySystem::GetComp<PlayerGC>(playerId)->attackSprite = weapons.cloneAnimation("player_sword");

	//EntitySystem::GetComp<PlayerLC>(playerId)->chooseSpawn();

	mode.loadData(2, 1, 144000);

	/*
	auto spawnables = stage.getSpawnables();
	EntitySystem::MakeComps<CapturePointGC>(spawnables.size(), spawnables.data());
	*/

	client.setPlayer(playerId);
	client.setWeaponManager(weapons);
	client.setClientPlayerSystem(&clientPlayers);
	client.setOnlineSystem(&online);
	client.setMode(&mode);
	client.setSpawns(&spawns);

	client.connect(address, port);
}

void Game::startStageEditor(const std::string & filePath) {
	for (auto& entity : EntitySystem::GetPool<EntityBaseComponent>()) {
		entity.isDead = true;
	}

	editables.isEnabled = true;
	editables.load(filePath);
}

void Game::loadStage(const std::string& stageName) {
	stage = Stage(stageName, spawns);
	stage.loadGraphics();
}

EntityId Game::getPlayerId() {
	return playerId;
}

const Stage& Game::getStage() const {
	return stage;
}
