#pragma once
#include "EntitySystem.h"
#include "NetworkTypes.h"
#include "PlayerLC.h"
#include <deque>

struct PlrContState {
	PlayerState plrState;
	unsigned char contState;
};

class ClientPlayerComponent {
public:
	ClientPlayerComponent(EntityId id = 0);
	const EntityId getid() const;

	void storePlayerState(Time_t gameTime, Time_t clientTime, Controller & controller);
	bool pollState(PlrContState & state);
	std::deque<PlrContState> readAllStates();
	Time_t getEarliestTime();
private:
	EntityId id;
	std::deque<PlrContState> states;
};