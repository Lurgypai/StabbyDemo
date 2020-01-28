#include "pch.h"
#include "ClientPlayerComponent.h"
#include "DebugFIO.h"

const int ClientPlayerComponent::MAX_STORED_STATES{50}
;
ClientPlayerComponent::ClientPlayerComponent(EntityId id_) :
	id{id_},
	netId{},
	states{}
{}

const EntityId ClientPlayerComponent::getid() const {
	return id;
}

void ClientPlayerComponent::storePlayerState(Time_t gameTime, Time_t clientTime, Controller& controller) {
	if (states.size() > MAX_STORED_STATES)
		states.pop_front();


	PlayerState playerState = EntitySystem::GetComp<PlayerLC>(id)->getState();
	playerState.gameTime = gameTime;
	playerState.clientTime = clientTime;
	states.emplace_back(PlrContState{ playerState, controller.getState() });

	//player state isn't set by physics, only by the update method in playerlc. thus, the effects of physics are not set. need to add a "getState" funtcion to the player, which sets all of the player state stuff and sends it.
	DebugFIO::Out("c_out.txt") << "stored state at time " << clientTime << ", pos: " << playerState.pos << ", vel: " << playerState.vel << '\n';
}

bool ClientPlayerComponent::pollState(PlrContState& state) {
	if (!states.empty()) {
		state = std::move(states.front());
		states.pop_front();
		return true;
	}
	return false;
}

std::deque<PlrContState> ClientPlayerComponent::readAllStates() {
	auto retStates = std::move(states);
	states.clear();
	return retStates;
}

Time_t ClientPlayerComponent::getEarliestTime() {
	return states.front().plrState.clientTime;
}
