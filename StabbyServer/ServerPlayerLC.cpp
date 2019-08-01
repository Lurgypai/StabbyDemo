#include "pch.h"
#include "ServerPlayerLC.h"
#include <algorithm>
#include "ServerClientData.h"
#include "PlayerStateComponent.h"
#include <iostream>

#include "SDL.h"
ServerPlayerLC::ServerPlayerLC(EntityId id_) :
	PlayerLC{id_},
	activeCommand{},
	prevStates{},
	clientTime{0},
	latestTime{0}
{
	prevStates.emplace_back(PlayerState{});
}

ServerPlayerLC::ServerPlayerLC(const ServerPlayerLC & other) :
	PlayerLC{other},
	activeCommand{other.activeCommand},
	prevStates{other.prevStates},
	clientTime{ other.clientTime },
	latestTime{ other.latestTime }
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
	if (c.when > latestTime) {
		commands.push_back(c);
		latestTime = c.when;
	}
}

void ServerPlayerLC::update(Time_t gameTime) {
	//update the client side time
	++clientTime;
	//std::cout << "updating for time: " << clientTime << '\n';

	int i = 0;
	for (; i != commands.size(); ++i) {
		if (commands[i].when <= clientTime)
			activeCommand = commands[i];
		else
			break;
	}
	commands.erase(commands.begin(), commands.begin() + i);

	//std::cout << "command for time: " << clientTime << "is from time " << activeCommand.when << '\n';

	PlayerLC::update(CLIENT_TIME_STEP, activeCommand.controllerState);

	if (prevStates.size() >= 32)
		prevStates.pop_front();

	PlayerStateComponent * stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
	stateComp->playerState.when = gameTime;
	prevStates.emplace_back(stateComp->playerState);
}
