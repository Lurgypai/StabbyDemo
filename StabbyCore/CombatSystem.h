#pragma once
#include "EntitySystem.h"
#include "PlayerLC.h"
#include "PhysicsComponent.h"
#include "Pool.h"

class CombatSystem {
public:
	CombatSystem();

	//check if any attackers hit any defenders
	template<typename Attacker, typename Defender>
	void runAttackCheck();
private:
	std::vector<EntityId> hitMobs;
	//index for the pool -> the entity who attacked
	//values in the vector -> who was attacked
	Pool<std::vector<EntityId>> hitList{};
	int prevAttackId;
};

template<typename Attacker, typename Defender>
inline void CombatSystem::runAttackCheck() {
	if (EntitySystem::Contains<Attacker>() && EntitySystem::Contains<Defender>()) {
		for (Attacker & attacker : EntitySystem::GetPool<Attacker>()) {
			for (Defender & defender : EntitySystem::GetPool<Defender>()) {
				EntityId attackerId = attacker.getId();
				EntityId defenderId = defender.getId();
				if (!hitList.contains(attackerId)) {
					hitList.add(attackerId, std::vector<EntityId>{});
				}

				//who we've hit
				std::vector<EntityId> & hitIds = hitList[attackerId];

				//if there has been a change in the attack state (started attacking, changed attack etc)
				if (attacker.readAttackChange()) {
					size_t priorSize = hitIds.size();
					hitIds.clear();
					hitIds.reserve(priorSize);
				}

				if (attacker.getActiveHitbox() != nullptr) {
					int count;
					const AABB * hurtboxes = defender.getHurtboxes(&count);
					for (int i = 0; i != count; ++i) {
						//if we are hitting them
						if (hurtboxes[i].intersects(*attacker.getActiveHitbox())) {
							//who we've already hit
							if (std::find(hitIds.begin(), hitIds.end(), defenderId) == hitIds.end()) {
								defender.damage(attacker.getActiveDamage());
								attacker.onAttackLand();

								hitIds.emplace_back(defenderId);
							}
						}
					}
				}
			}
		}
	}
}
