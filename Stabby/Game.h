#pragma once
#include <string>

#include "Client.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PickupSystem.h"
#include "WeaponManager.h"
#include "ClimbableSystem.h"
#include "EditableSystem.h"

class Game {
public:
	void startOfflineGame(const std::string& stage);
	void startOnlineGame(const std::string & address, int port);
	void startStageEditor(const std::string & filePath);
	void loadStage(const std::string& stageName);

	//client side time
	Time_t tick;
	Client client;
	PhysicsSystem physics;
	CombatSystem combat;
	PickupSystem pickups;
	WeaponManager weapons;
	ClimbableSystem climbables;
	EditableSystem editables;

	int playerCamId;
	int debugCamId;
	int editorCamId;

	EntityId getPlayerId();
	const Stage& getStage() const;

private:
	Stage stage;
	EntityId playerId;
};