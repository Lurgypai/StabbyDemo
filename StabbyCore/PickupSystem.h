#pragma once
#include "EntitySystem.h"
#include "AABB.h"

class PickupSystem {
public:
	template<typename Pickup, typename Target>
	void runPickupCheck();
};

template<typename Pickup, typename Target>
inline void PickupSystem::runPickupCheck() {
	if (EntitySystem::Contains<Pickup>() && EntitySystem::Contains<Target>()) {

		std::vector<EntityId> toRemoveIds;
		toRemoveIds.reserve(EntitySystem::GetPool<Pickup>().size());

		for (auto & pickup : EntitySystem::GetPool<Pickup>()) {
			pickup.updateColliders();
			for (auto & target : EntitySystem::GetPool<Target>()) {
				int targetCollidersSize;
				int pickupCollidersSize;

				const AABB * targetHurtboxes = target.getHurtboxes(&targetCollidersSize);
				const AABB * pickupColliders = pickup.getColliders(&pickupCollidersSize);

				for (int i = 0; i != targetCollidersSize; ++i) {
					const AABB & targetCollider = targetHurtboxes[i];
					for (int j = 0; j != pickupCollidersSize; ++j) {
						const AABB & pickupCollider = pickupColliders[j];
						if (targetCollider.intersects(pickupCollider)) {
							pickup.onPickup<Target>(target.getId());
							toRemoveIds.push_back(pickup.getId());
						}
					}
				}
			}
		}

		for (auto & id : toRemoveIds) {
			EntitySystem::GetComp<Pickup>(id)->die();
		}
	}
}
