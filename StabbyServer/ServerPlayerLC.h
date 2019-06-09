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
	Time_t when;
};

class ServerPlayerLC : public PlayerLC {
public:
	ServerPlayerLC( EntityId id_ = 0 );
	ServerPlayerLC(const ServerPlayerLC & other);
	PlayerState getStateAt(Time_t gameTime);
	void bufferInput(ClientCommand c);
	void update(Time_t gameTime);
	//when the most recent server update happened client side (it already happened on the client)
	Time_t clientTime;

private:
	ClientCommand latest;
	std::deque<PlayerState> prevStates;
};