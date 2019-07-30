#pragma once
#include "RenderComponent.h"

class HealthPickupGC {
public:
	HealthPickupGC(EntityId id_ = 0);
	void updateState(double timeDelta);

	EntityId getId() const;
private:
	EntityId id;
};