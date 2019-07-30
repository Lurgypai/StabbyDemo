#pragma once
#include <string>

#include "Client.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PickupSystem.h"

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
	Stage stage;

	EntityId getPlayerId();

private:
	EntityId playerId;
};