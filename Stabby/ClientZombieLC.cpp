#include "pch.h"
#include "ClientZombieLC.h"
#include "ClientPlayerLC.h"
#include <iostream>

#define MAX_QUEUE_SIZE 20

ClientZombieLC::ClientZombieLC(EntityId id_) :
	ZombieLC{id_},
	last{0},
	start{0},
	end{0}
{
	prevStates.resize(MAX_QUEUE_SIZE);
}

void ClientZombieLC::runLogic(Time_t now) {
	ZombieLC::runLogic();

	zombieState.when = now;
	
	//circular queue
	++end;
	if (end - start > MAX_QUEUE_SIZE)
		++start;

	prevStates[end % MAX_QUEUE_SIZE] = zombieState;
}

/*
void ClientZombieLC::repredict(ZombieState otherState, Time_t when) {
	if (when > last) {
		if (start < end) {
			for (int i = start % MAX_QUEUE_SIZE; i != end % MAX_QUEUE_SIZE; ++i) {
				auto predictedState = prevStates[i];
				if (predictedState.when == when) {
					if (predictedState != otherState) {
						/*
						std::cout << "Zombie prediction failed.\n";
						if (predictedState.facing != otherState.facing)
							std::cout << "z-facing: " << predictedState.facing << ", " << otherState.facing << '\n';
						if (predictedState.health != otherState.health)
							std::cout << "z-health: " << predictedState.health << ", " << otherState.health << '\n';
						if (predictedState.relaxFrame != otherState.relaxFrame)
							std::cout << "z-relaxFrame: " << predictedState.relaxFrame << ", " << otherState.relaxFrame << '\n';
						if (predictedState.stunFrame != otherState.stunFrame)
							std::cout << "z-stunFrame: " << predictedState.stunFrame << ", " << otherState.stunFrame << '\n';
						if (predictedState.chargeFrame != otherState.chargeFrame)
							std::cout << "z-chargeFrame: " << predictedState.chargeFrame << ", " << otherState.chargeFrame << '\n';
						if (predictedState.attackFrame != otherState.attackFrame)
							std::cout << "z-attackFrame: " << predictedState.attackFrame << ", " << otherState.attackFrame << '\n';
						if (predictedState.attackChanged != otherState.attackChanged)
							std::cout << "z-attackChanged: " << predictedState.attackChanged << ", " << otherState.attackChanged << '\n';
						if (predictedState.state != otherState.state)
							std::cout << "z-state: " << static_cast<int>(predictedState.state) << ", " << static_cast<int>(otherState.state) << '\n';
						if (predictedState.pos != otherState.pos)
							std::cout << "z-pos: " << predictedState.pos << ", " << otherState.pos << '\n';
						if (predictedState.vel != otherState.vel)
							std::cout << "z-vel: " << predictedState.vel << ", " << otherState.vel << '\n';
						*
						setState(otherState);
						prevStates.erase(prevStates.begin(), prevStates.begin() + i + 1);
						std::deque<ZombieState> toUpdate{ std::move(prevStates) };
						prevStates.clear();
						for (auto& state : toUpdate) {
							searchForTarget<ClientPlayerLC>();
							runLogic(state.when);
						}
					}

					last = when;
					break;
				}
			}
		}

		if (!prevStates.empty() && prevStates.back().when < when) {
			std::cout << "Zombie behind.\n";
			setState(otherState);
			prevStates.clear();
			last = when;
		}
	}
}
*/
