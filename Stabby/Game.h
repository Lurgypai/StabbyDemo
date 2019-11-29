#pragma once
#include <string>

#include "Client.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PickupSystem.h"
#include "WeaponManager.h"
#include "ClimbableSystem.h"

class Game {
public:
	void startOfflineGame();
	void startOnlineGame(const std::string & address, int port);

	//client side time
	Time_t tick;
	Client client;
	PhysicsSystem physics;
	CombatSystem combat;
	PickupSystem pickups;
	WeaponManager weapons;
	Stage stage;
	ClimbableSystem climbables;

	EntityId getPlayerId();

private:
	EntityId playerId;
};