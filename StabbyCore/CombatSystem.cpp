#include "stdafx.h"
#include "CombatSystem.h"
#include "PhysicsComponent.h"
#include "DirectionComponent.h"

CombatSystem::CombatSystem() {}

void CombatSystem::runAttackCheck(double timeDelta) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent & attacker : EntitySystem::GetPool<CombatComponent>()) {

			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
			DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

			attacker.updateHurtboxes();
			if(!physics->frozen)
				attacker.attack.update(timeDelta, physics->getPos(), direction->dir);

			for (CombatComponent & defender : EntitySystem::GetPool<CombatComponent>()) {
				if (defender.getId() != attacker.getId()) {
					EntityId attackerId = attacker.getId();
					EntityId defenderId = defender.getId();
					if (!hitList.contains(attackerId)) {
						hitList.add(attackerId, std::vector<EntityId>{});
					}

					//who we've hit
					std::vector<EntityId> & hitIds = hitList[attackerId];

					//if there has been a change in the attack state (started attacking, changed attack etc)
					if (attacker.attack.pollAttackChange()) {
						size_t priorSize = hitIds.size();
						hitIds.clear();
						hitIds.reserve(priorSize);
					}
					if (attacker.teamId != defender.teamId) {
						if (attacker.getActiveHitbox() != nullptr) {
							const AABB & hitbox = attacker.getActiveHitbox()->hit;
							if (hitbox.intersects(defender.getBoundingBox())) {
								auto hurtboxes = defender.hurtboxes;
								for (auto & hurtbox : hurtboxes) {
									if (hurtbox.box.intersects(hitbox)) {
										//who we've already hit
										if (std::find(hitIds.begin(), hitIds.end(), defenderId) == hitIds.end()) {
											defender.damage(attacker.rollDamage());
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
		}
	}
}
