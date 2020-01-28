#pragma once
#include "EntitySystem.h"
#include "Controller.h"
#include "WeaponManager.h"
#include "Stage.h"

class PlayerManager {
public:
	EntityId makePlayer(const WeaponManager & weapons);

	void updateAll(double timeDelta, const Controller & cont, const Stage & stage);
	void update(EntityId player, double timeDelta, const Controller& cont, const Stage& stage);
};