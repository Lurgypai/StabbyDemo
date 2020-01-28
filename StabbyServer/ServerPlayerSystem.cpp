#include "pch.h"
#include "ServerPlayerSystem.h"
#include "EntitySystem.h"
#include <ServerPlayerComponent.h>
#include "PlayerLC.h"
#include "ServerClientData.h"
#include "DebugFIO.h"

void ServerPlayerSystem::updatePlayers(PlayerManager & players, Time_t gameTime, const Stage & stage) {
	if (EntitySystem::Contains<ServerPlayerComponent>()) {
		for (auto& onlinePlayer : EntitySystem::GetPool<ServerPlayerComponent>()) {
			EntityId id = onlinePlayer.getId();
			ClientCommand latestCommand = onlinePlayer.readCommand(gameTime);

			players.update(id, CLIENT_TIME_STEP, latestCommand.controllerState, stage);
		}
	}
	//get the controller state for each player
	//update with that state
}

void ServerPlayerSystem::tickPlayerTimes() {
	if (EntitySystem::Contains<ServerPlayerComponent>()) {
		for (auto& player : EntitySystem::GetPool<ServerPlayerComponent>()) {
			player.tickClientTime();
		}
	}
}
