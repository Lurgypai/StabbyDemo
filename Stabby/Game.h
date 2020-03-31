#pragma once
#include <string>

#include "Client.h"
#include "PhysicsSystem.h"
#include "CombatSystem.h"
#include "PickupSystem.h"
#include "WeaponManager.h"
#include "ClimbableSystem.h"
#include "EditableSystem.h"
#include "PlayerManager.h"
#include "ClientPlayerSystem.h"
#include "PaletteManager.h"
#include "SpawnSystem.h"
#include "DominationMode.h"
#include "OnlineSystem.h"

class Game {
public:
	Game();
	void startOfflineGame(const std::string& stage);
	void startOnlineGame(const std::string & address, int port, const std::string & stageName);
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
	PlayerManager players;
	ClientPlayerSystem clientPlayers;
	PaletteManager palettes;
	SpawnSystem spawns;
	DominationMode mode;
	OnlineSystem online;

	int playerCamId;
	int debugCamId;
	int editorCamId;

	EntityId getPlayerId();
	const Stage& getStage() const;

private:
	Stage stage;
	EntityId playerId;
};