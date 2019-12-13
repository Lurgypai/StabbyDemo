#include "pch.h"
#include "ClientPlayerLC.h"
#include "ServerClientData.h"
#include "PlayerStateComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"
#include <algorithm>
#include <iostream>
#include "DebugFIO.h"

ClientPlayerLC::ClientPlayerLC(EntityId id_) :
	PlayerLC{id_},
	last{0}
{}

void ClientPlayerLC::update(Time_t clientTime, Time_t gameTime, double timeDelta, const Controller & controller) {
	PlayerLC::update(timeDelta, controller);

	//maintain constant max size
	if (states.size() >= BUFFER_SIZE)
		states.pop_front();

	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	playerState->playerState.gameTime = gameTime;
	playerState->playerState.clientTime = clientTime;
	states.emplace_back(TotalPlayerState{ playerState->playerState, controller.getState() });

	//print the stored state
	//DebugFIO::Out("c_out.txt") << "pos at time " << clientTime << ": " << playerState->playerState.pos << '\n';
	//DebugFIO::Out("c_out.txt") << "vel at time " << clientTime << ": " << playerState->playerState.vel << '\n';
	//DebugFIO::Out("c_out.txt") << "inp at time " << clientTime << ": " << static_cast<int>(controller.getState()) << '\n';
}

//this doesn't run physics - will need to be fixed
bool ClientPlayerLC::repredict(const PlayerState & state) {
	bool ret = false;
	if (state.clientTime > last) {
		for (auto i = 0; i != states.size(); ++i) {
			auto tstate = states[i];
			if (tstate.plr.clientTime == state.clientTime) {

				tstate.plr.gameTime = state.gameTime;

				states.erase(states.begin(), states.begin() + i + 1);

				//std::ofstream output{ "c_log.txt", std::ios::app };

				std::ostream& out = std::cout;

				if (tstate.plr != state) {
					DebugFIO::Out("c_out.txt") << "Prediction failed, resetting at time: " << state.clientTime << '\n';
					if (state.state != tstate.plr.state)
						out << "state: " << static_cast<int>(tstate.plr.state) << ", " << static_cast<int>(state.state) << '\n';
					if (state.rollFrame != tstate.plr.rollFrame)
						out << "rollFrame: " << tstate.plr.rollFrame << ", " << state.rollFrame << '\n';
					if (state.activeAttack != tstate.plr.activeAttack)
						out << "activeAttack: " << tstate.plr.activeAttack << ", " << state.activeAttack << '\n';
					if (state.attackFrame != tstate.plr.attackFrame)
						out << "attackFrame: " << tstate.plr.attackFrame << ", " << state.attackFrame << '\n';
					if (state.health != tstate.plr.health)
						out << "health: " << tstate.plr.health << ", " << state.health << '\n';
					if (state.stunFrame != tstate.plr.stunFrame)
						out << "stunFrame: " << tstate.plr.stunFrame << ", " << state.stunFrame << '\n';
					if (state.facing != tstate.plr.facing)
						out << "facing: " << tstate.plr.facing << ", " << state.facing << '\n';
					if (state.spawnPoint != tstate.plr.spawnPoint)
						out << "spawnPoint: " << tstate.plr.spawnPoint << ", " << state.spawnPoint << '\n';
					if (state.attackFreezeFrame != tstate.plr.attackFreezeFrame)
						out << "attackFreezeFrame: " << tstate.plr.attackFreezeFrame << ", " << state.attackFreezeFrame << '\n';
					if (state.healFrame != tstate.plr.healFrame)
						out << "healFrame" << tstate.plr.healFrame << ", " << state.healFrame << '\n';
					if (state.pos != tstate.plr.pos)
						out << "pos: " << tstate.plr.pos << ", " << state.pos << '\n';
					if (state.vel != tstate.plr.vel)
						out << "vel: " << tstate.plr.vel << ", " << state.vel << '\n';
					if(state.frozen != tstate.plr.frozen)
						out << "frozen: " << tstate.plr.frozen << ", " << state.frozen << '\n';

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

					DirectionComponent* dir = EntitySystem::GetComp<DirectionComponent>(id);
					dir->dir = state.facing;

					DebugFIO::Out("c_out.txt") << "reset pos at time " << state.clientTime << " to: " << physics->getPos() << '\n';
					DebugFIO::Out("c_out.txt") << "reset vel at time " << state.clientTime << " to: " << physics->vel << '\n';

					//the state we are reset to is from before physics/combat are applied, so these have to be re-applied after the state is fixed
					physicsSystem->runPhysics(CLIENT_TIME_STEP, id);
					combatSystem->runAttackCheck(CLIENT_TIME_STEP, id);

					std::deque<TotalPlayerState> toUpdate{ std::move(states) };
					states.clear();

					//now reevaulate, this will refill states
					for (auto& updateState : toUpdate) {
						update(updateState.plr.clientTime, updateState.plr.gameTime, CLIENT_TIME_STEP, Controller{updateState.in});
						physicsSystem->runPhysics(CLIENT_TIME_STEP, id);
						combatSystem->runAttackCheck(CLIENT_TIME_STEP, id);
					}

					for (auto& tstate_ : states) {
						DebugFIO::Out("c_out.txt") << "recalculated pos at time " << tstate_.plr.clientTime << " to: " << tstate_.plr.pos << '\n';
						DebugFIO::Out("c_out.txt") << "recalculated vel at time " << tstate_.plr.clientTime << " to: " << tstate_.plr.vel << '\n';
						DebugFIO::Out("c_out.txt") << "using        inp at time " << tstate_.plr.clientTime << " to: " << static_cast<int>(tstate_.in) << '\n';
					}
					
					ret = true;
				}
				last = state.clientTime;
				break;
			}
		}
	}
	return ret;
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
