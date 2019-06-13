#include "pch.h"
#include "Game.h"
#include "ClientPlayerLC.h"
#include "PlayerGC.h"
#include "PlayerCam.h"
#include "ZombieGC.h"
#include "ZombieLC.h"

void Game::startOfflineGame() {
	//load up the stage
	//grab the  player spawn point
	//create the player there
	
	Vec2f spawnPos = stage.getSpawnPos();

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<ClientPlayerLC>(1, &playerId);

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<PlayerGC>(playerId)->loadSprite<AnimatedSprite>("images/stabbyman_with_hilt.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	pos->teleport( spawnPos );
}

void Game::startOnlineGame(const std::string & address, int port) {
	client.connect(time, address, port);
	
	Vec2f spawnPos = stage.getSpawnPos();

	EntitySystem::GenEntities(1, &playerId);
	EntitySystem::MakeComps<ClientPlayerLC>(1, &playerId);
	EntitySystem::GetComp<ClientPlayerLC>(playerId)->setPhysics(physics);

	EntitySystem::MakeComps<PlayerGC>(1, &playerId);
	EntitySystem::GetComp<PlayerGC>(playerId)->loadSprite<AnimatedSprite>("images/stabbyman_with_hilt.png", Vec2i{ 64, 64 });
	EntitySystem::GetComp<PlayerGC>(playerId)->loadAnimations();

	PhysicsComponent * pos = EntitySystem::GetComp<PhysicsComponent>(playerId);
	pos->teleport(spawnPos);

	client.setPlayer(playerId);
}

EntityId Game::getPlayerId() {
	return playerId;
}
