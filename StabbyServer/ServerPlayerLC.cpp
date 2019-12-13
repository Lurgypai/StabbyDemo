#include "pch.h"
#include "ServerPlayerLC.h"
#include <algorithm>
#include "ServerClientData.h"
#include "PlayerStateComponent.h"
#include <iostream>
#include "DebugFIO.h"

#include "SDL.h"
ServerPlayerLC::ServerPlayerLC(EntityId id_) :
	PlayerLC{id_},
	activeCommand{},
	prevStates{},
	latestTime{0},
	clientTime{0},
	clientTimeUpToDate{false}
{
	prevStates.emplace_back(PlayerState{});
}

PlayerState ServerPlayerLC::getStateAt(Time_t gameTime) {
	for (auto iter = prevStates.rbegin(); iter != prevStates.rend(); ++iter) {
		if (iter->gameTime < gameTime)
			if (iter != prevStates.rbegin())
				return(*iter--);
			else
				return *iter;
	}
	return prevStates.front();
}

PlayerState ServerPlayerLC::getLatestState() {
	return prevStates.back();
}

void ServerPlayerLC::bufferInput(ClientCommand c) {
	//skip inputs older than the last one
	//keep inputs older than the current time (clientTime). Avoid dropping inputs (though this will cause desyncs if thigns are too old.)
	if (c.clientTime > latestTime) {

		Time_t delay = ((1 / CLIENT_TIME_STEP) * SERVER_TIME_STEP);
		if (c.clientTime < clientTime || clientTime < c.clientTime - (delay * 2)) {
			//delay by one server tick, so all incoming updates are properly in time.
			clientTime = c.clientTime - delay;
		}

		commands.push_back(c);
		latestTime = c.clientTime;
		//DebugFIO::Out("s_out.txt") << "received input " << static_cast<int>(c.controllerState.getState()) << " for time " << c.clientTime << '\n';
	}
}

void ServerPlayerLC::update(Time_t gameTime) {

		//remove find the mose recent command
		int i = 0;
		for (; i != commands.size(); ++i) {
			if (commands[i].clientTime <= clientTime)
				activeCommand = commands[i];
			else
				break;
		}
		commands.erase(commands.begin(), commands.begin() + i);

		//update and store
		auto state = activeCommand.controllerState.getState();

		PlayerLC::update(CLIENT_TIME_STEP, activeCommand.controllerState);

		if (prevStates.size() >= 32)
			prevStates.pop_front();

		PlayerStateComponent* stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
		stateComp->playerState.gameTime = gameTime;
		stateComp->playerState.clientTime = clientTime;
		prevStates.emplace_back(stateComp->playerState);

		//print the stored pos and vel, with the time linked.
		//auto& out = DebugFIO::Out("s_out.txt");
		//out << "pos at time " << clientTime << ": " << stateComp->playerState.pos << '\n';
		//out << "vel at time " << clientTime << ": " << stateComp->playerState.vel << '\n';
		//out << "inp at time " << clientTime << ": " << static_cast<int>(activeCommand.controllerState.getState()) << '\n';

		++clientTime;
}

//set to use game time
