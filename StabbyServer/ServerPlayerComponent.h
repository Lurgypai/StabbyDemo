#pragma once
#include "EntitySystem.h"
#include "Controller.h"
#include "NetworkTypes.h"
#include <deque>

struct ClientCommand {
	Controller controllerState;
	Time_t clientTime;
	Time_t gameTime;
};

class ServerPlayerComponent {
public:
	ServerPlayerComponent(EntityId id_ = 0);
	const EntityId getId() const;

	Time_t getClientTime();
	void bufferInput(ClientCommand c);
	ClientCommand readCommand(Time_t gameTime);
	void tickClientTime();
private:
	Time_t latestTime;
	Time_t clientTime;
	std::deque<ClientCommand> commands;
	ClientCommand latestCommand;
	EntityId id;
};