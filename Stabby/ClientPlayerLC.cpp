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
	playerState->playerState.when = now;
	states.emplace_back(TotalPlayerState{ playerState->playerState, controller.getState() });
}

//this doesn't run physics - will need to be fixed
void ClientPlayerLC::repredict(const PlayerState & state) {
	if (state.when > last) {
		for (auto i = 0; i != states.size(); ++i) {
			auto tstate = states[i];
			if (tstate.plr.when == state.when) {
				//reevaulate current pos
				auto backup = states;
				states.erase(states.begin(), states.begin() + i + 1);
				if (tstate.plr != state) {
					std::cout << "Prediction failed, re-predicting at time: " << state.when << '\n';
					if (state.state != tstate.plr.state)
						std::cout << "state: " << static_cast<int>(tstate.plr.state) << ", " << static_cast<int>(state.state) << '\n';
					if (state.rollFrame != tstate.plr.rollFrame)
						std::cout << "rollFrame: " << tstate.plr.rollFrame << ", " << state.rollFrame << '\n';
					if (state.activeAttack != tstate.plr.activeAttack)
						std::cout << "activeAttack: " << tstate.plr.activeAttack << ", " << state.activeAttack << '\n';
					if (state.attackFrame != tstate.plr.attackFrame)
						std::cout << "attackFrame: " << tstate.plr.attackFrame << ", " << state.attackFrame << '\n';
					if (state.health != tstate.plr.health)
						std::cout << "health: " << tstate.plr.health << ", " << state.health << '\n';
					if (state.stunFrame != tstate.plr.stunFrame)
						std::cout << "stunFrame: " << tstate.plr.stunFrame << ", " << state.stunFrame << '\n';
					if (state.facing != tstate.plr.facing)
						std::cout << "facing: " << tstate.plr.facing << ", " << state.facing << '\n';
					if (state.spawnPoint != tstate.plr.spawnPoint)
						std::cout << "spawnPoint: " << tstate.plr.spawnPoint << ", " << state.spawnPoint << '\n';
					if (state.attackFreezeFrame != tstate.plr.attackFreezeFrame)
						std::cout << "attackFreezeFrame: " << tstate.plr.attackFreezeFrame << ", " << state.attackFreezeFrame << '\n';
					if (state.healFrame != tstate.plr.healFrame)
						std::cout << "healFrame" << tstate.plr.healFrame << ", " << state.healFrame << '\n';
					if (state.pos != tstate.plr.pos)
						std::cout << "pos: " << tstate.plr.pos << ", " << state.pos << '\n';
					if (state.vel != tstate.plr.vel)
						std::cout << "vel: " << tstate.plr.vel << ", " << state.vel << '\n';
					if(state.frozen != tstate.plr.frozen)
						std::cout << "frozen: " << tstate.plr.frozen << ", " << state.frozen << '\n';

					PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
					playerState->playerState = state;
					PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
					physics->vel = state.vel;
					physics->teleport(state.pos);
					physics->frozen = state.frozen;

					CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);
					Attack & attack = combat->attack;
					attack.setActive(state.activeAttack);
					attack.setFrame(state.attackFrame);
					combat->health = state.health;
					combat->stunFrame = state.stunFrame;

					//move our remaining values out, and then clear
					std::deque<TotalPlayerState> toUpdate{ std::move(states) };
					states.clear();

					//now reevaulate, this will refill states
					for (auto& updateState : toUpdate) {
						combatSystem->runAttackCheck(CLIENT_TIME_STEP, id);
						physicsSystem->runPhysics(CLIENT_TIME_STEP, id);
						update(updateState.plr.when, CLIENT_TIME_STEP, Controller{updateState.in});
						//add combat updates to this
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
			std::cout << "Client behind the server, updating positions.\n";

			PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
			playerState->playerState = state;

			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->vel = state.vel;
			physics->teleport(state.pos);
			physics->frozen = state.frozen;


			CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);
			Attack & attack = combat->attack;
			attack.setActive(state.activeAttack);
			attack.setFrame(state.attackFrame);

			states.clear();
			last = state.when;
		}
	}
}

void ClientPlayerLC::setPhysics(PhysicsSystem & physics_) {
	physicsSystem = &physics_;
}

void ClientPlayerLC::setCombat(CombatSystem& combat) {
	combatSystem = &combat;
}

std::string ClientPlayerLC::getHeadPath() {
	return "images/head.png";
}

Vec2f ClientPlayerLC::getCenter() {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	return physics->center();
}
