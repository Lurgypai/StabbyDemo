#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class HealthPickupLC {
public:
	HealthPickupLC(EntityId id_ = 0);
	EntityId getId() const;

	template<typename Player>
	void onPickup(EntityId playerId);

	void update(time_t clientTime, double timeDelta);

	void updateColliders();
	void die();
	AABB * getColliders(int * size);

	int amount;
private:
	EntityId id;
	AABB collider;
};

template<typename Player>
inline void HealthPickupLC::onPickup(EntityId playerId) {
	if (EntitySystem::Contains<Player>()) {
		Player * player = EntitySystem::GetComp<Player>(playerId);
		player->heal(amount);
	}
}
