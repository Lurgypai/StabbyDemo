#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "PlayerLC.h"
#define INPUT_QUEUE_SIZE 500
#include <vector>
#include <deque>
#include "PlayerStateComponent.h"

struct ClientCommand {
	Controller controllerState;
	Time_t clientTime;
	Time_t gameTime;
};

class ServerPlayerLC : public PlayerLC {
public:
	ServerPlayerLC( EntityId id_ = 0 );
	PlayerState getStateAt(Time_t gameTime);
	PlayerState getLatestState();
	void bufferInput(ClientCommand c);
	void update(Time_t gameTime);
private:
	ClientCommand activeCommand;
	Time_t latestTime;
	Time_t clientTime;
	bool clientTimeUpToDate;
	std::deque<ClientCommand> commands;
	std::deque<PlayerState> prevStates;

};