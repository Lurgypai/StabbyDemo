#include "pch.h"
#include "ServerPlayerLC.h"
#include <algorithm>
#include "ServerClientData.h"
#include "PlayerStateComponent.h"
#include <iostream>

#include "SDL.h"
ServerPlayerLC::ServerPlayerLC(EntityId id_) :
	PlayerLC{id_},
	latest{},
	prevStates{},
	clientTime{0}
{
	prevStates.emplace_back(PlayerState{});
}

ServerPlayerLC::ServerPlayerLC(const ServerPlayerLC & other) :
	PlayerLC{other},
	latest{other.latest},
	prevStates{other.prevStates},
	clientTime{ other.clientTime }
{}

PlayerState ServerPlayerLC::getStateAt(Time_t gameTime) {
	for (auto iter = prevStates.rbegin(); iter != prevStates.rend(); ++iter) {
		if (iter->when < gameTime)
			if (iter != prevStates.rbegin())
				return(*iter--);
			else
				return *iter;
	}
	return prevStates.front();
}

void ServerPlayerLC::bufferInput(ClientCommand c) {
	if (c.when > latest.when) {
		latest = c;
		clientTime = latest.when - 1; 
	}
}

void ServerPlayerLC::update(Time_t gameTime) {
	//update the client side time
	++clientTime;
	PlayerLC::update(CLIENT_TIME_STEP, latest.controllerState);

	if (prevStates.size() >= 32)
		prevStates.pop_front();

	PlayerStateComponent * stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
	stateComp->setWhen(gameTime);
	prevStates.emplace_back(stateComp->getPlayerState());
}
