#pragma once
#include "NetworkTypes.h"
#include "EntitySystem.h"
#include "Controller.h"
#include "PlayerStateComponent.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include <unordered_map>

//contains the last time a ClientPlayer was updated, and their EntityId
struct LastClientTick {
	Time_t clientTime;
	Time_t gameTime;
	EntityId player;
};

class ClientPlayerSystem {
public:
	ClientPlayerSystem(PhysicsSystem* physicsSystem_, CombatSystem* combatSystem_);
	void update(Time_t gameTime, Time_t clientTime);
	void update(Time_t gameTime, Time_t clientTime, const EntityId& player);
	void repredict(EntityId playerId, NetworkId netId, const PlayerState& state, double timeDelta);
private:
	PhysicsSystem* physicsSystem;
	CombatSystem* combatSystem;
	std::unordered_map<NetworkId, LastClientTick> players;
};