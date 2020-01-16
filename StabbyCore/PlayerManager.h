#pragma once
#include "EntitySystem.h"
#include "Controller.h"
#include "WeaponManager.h"
#include "Stage.h"

class PlayerManager {
public:
	EntityId makePlayer(const WeaponManager & weapons);
	void makeServerPlayer();
	void makeClientPlayer();
	void makeOnlinePlayer();

	void update(double timeDelta, const Controller & cont, const Stage & stage);
};