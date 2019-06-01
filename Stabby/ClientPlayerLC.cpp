#include "pch.h"
#include "ClientPlayerLC.h"
#include "ServerClientData.h"
#include "PlayerStateComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include <algorithm>
#include <iostream>

ClientPlayerLC::ClientPlayerLC(EntityId id_) :
	PlayerLC{id_},
	last{0}
{}

void ClientPlayerLC::update(Time_t now, double timeDelta, const Controller & controller) {
	PlayerLC::update(timeDelta, controller);

	//maintain constant max size
	if (states.size() >= BUFFER_SIZE)
		states.pop_front();

	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	playerState->setWhen(now);
	states.emplace_back(TotalPlayerState{ playerState->getPlayerState(), controller.getState() });
}

void ClientPlayerLC::repredict(const PlayerState & state, const Stage& stage) {
	if (state.when > last) {
		for (auto i = 0; i != states.size(); ++i) {
			auto tstate = states[i];
			if (tstate.plr.when == state.when) {
				//reevaulate current pos
				states.erase(states.begin(), states.begin() + i + 1);
				if (tstate.plr != state) {
					std::cout << "Prediction failed, re-predicting\n";

					PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
					playerState->setPlayerState(state);

					PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
					position->pos = state.pos;

					PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
					physics->vel = state.vel;

					//move our remaining values out, and then clear
					std::deque<TotalPlayerState> toUpdate{ std::move(states) };
					states.clear();

					//now reevaulate, this will refill states
					for (auto& updateState : toUpdate) {
						update(updateState.plr.when, CLIENT_TIME_STEP, Controller{updateState.in});
					}
				}

				last = state.when;
				break;
			}
		}
		//if the for loop exits one of two things occured
		//the state we received was so old it wasn't in states
		//the state we received was after our latest update.
		//in the latter case, give a warning, and update to where it says we should be

		//we need to send the current time to the server when this happens, as it can happen because of window movements etc
		if (!states.empty() && states.back().plr.when < state.when) {
			std::cout << "We're somehow behind the server! Updating to match.\n";

			PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
			playerState->setPlayerState(state);

			PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
			position->pos = state.pos;

			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->vel = state.vel;

			states.clear();
			last = state.when;
		}
	}
}

std::string ClientPlayerLC::getHeadPath() {
	return "images/head.png";
}

Vec2f ClientPlayerLC::getCenter() {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	return physics->center();
}
