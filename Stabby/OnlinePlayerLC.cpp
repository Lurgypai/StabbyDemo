#include "pch.h"
#include "OnlinePlayerLC.h"
#include <iostream>
#include "SDL.h"
#include "ServerClientData.h"
#include "PositionComponent.h"

OnlinePlayerLC::OnlinePlayerLC(EntityId id_) :
	id{ id_ },
	netId{ 0 },
	previousPos{},
	whens{}
{
	if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PositionComponent>(1, &id);
	}
	if (!EntitySystem::Contains<PlayerStateComponent>() || EntitySystem::GetComp<PlayerStateComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PlayerStateComponent>(1, &id);
		EntitySystem::GetComp<PlayerStateComponent>(id)->playerState.facing = 1;
	}
}

void OnlinePlayerLC::setNetId(NetworkId id_) {
	netId = id_;
}

NetworkId OnlinePlayerLC::getNetId() {
	return netId;
}

EntityId OnlinePlayerLC::getId() const {
	return id;
}

void OnlinePlayerLC::interp(PlayerState st, Time_t when) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerStateComponent *playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PlayerState & state = playerState->playerState;

	state = st;

	if (state.vel.x < 0)
		state.facing = -1;
	else if(state.vel.x > 0)
		state.facing = 1;

	previousPos[0] = previousPos[1];
	previousPos[1] = previousPos[2];
	previousPos[2] = st.pos;

	whens[0] = whens[1];
	whens[1] = whens[2];
	whens[2] = when;

	state.pos = previousPos[1];
	position->pos = previousPos[1] - Vec2f{static_cast<float>(PlayerLC::PLAYER_WIDTH) / 2, static_cast<float>(PlayerLC::PLAYER_HEIGHT)};
}

void OnlinePlayerLC::update(Time_t gameTime) {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	//switch to server ticks
	//this is how long we want to tak to get from where we are to where we are going
	int front = 2;
	if (gameTime > whens[front] && (gameTime - whens[front]) * GAME_TIME_STEP < 0.1)
		front = 1;


	double delta = static_cast<double>(whens[front] - whens[front - 1]) * GAME_TIME_STEP;
	//this is how long it is between each update.

	Vec2f moveDistance = (previousPos[front] - previousPos[front - 1]) * static_cast<float>(CLIENT_TIME_STEP / delta);
	position->pos += moveDistance;
}
