#pragma once
#include <string>

#include "Client.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PickupSystem.h"
#include "AttackManager.h"

class Game {
public:
	void startOfflineGame();
	void startOnlineGame(const std::string & address, int port);

	//client side time
	Time_t time;
	Client client;
	PhysicsSystem physics;
	CombatSystem combat;
	PickupSystem pickups;
	AttackManager attacks;
	Stage stage;

	EntityId getPlayerId();

private:
	EntityId playerId;
};