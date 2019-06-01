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

//check if we hit anyone else from ze past
void ServerPlayerLC::runHitDetect(Time_t gameTime) {
	Time_t timeToFind = gameTime - (.1 * GAME_TIME_STEP);
	PlayerState stateWhenAction = getStateAt(timeToFind);

	if (stateWhenAction.state != State::rolling) {

		PhysicsComponent * physics = getPhysics();

		AABB collider = { stateWhenAction.pos, physics->getRes() };

		bool wasHit = false;
		for (auto& playerComp : EntitySystem::GetPool<ServerPlayerLC>()) {
			if (playerComp.id != id) {
				auto& player = playerComp;
				if (player.getAttack().getActive() != nullptr && collider.intersects(player.getAttack().getActive()->hit)) {
					if (!isBeingHit) {
						damage(1);
					}
					wasHit = true;
				}
			}
		}
		isBeingHit = wasHit;
	}
}
