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
			attacker.updateStun();
			if(physics != nullptr && !physics->frozen)
				attacker.attack.update(timeDelta, physics->getPos(), direction->dir);

			for (CombatComponent & defender : EntitySystem::GetPool<CombatComponent>()) {
				attackCheck(attacker, defender);
			}
		}
	}
}

void CombatSystem::runAttackCheck(double timeDelta, EntityId id) {
	if (EntitySystem::Contains<CombatComponent>()) {
		for (CombatComponent& attacker : EntitySystem::GetPool<CombatComponent>()) {

			PhysicsComponent* physics = EntitySystem::GetComp<PhysicsComponent>(attacker.getId());
			DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(attacker.getId());

			//if we're the attacker, loop through all the defenders and attack them
			if (attacker.getId() == id) {
				attacker.updateHurtboxes();
				attacker.updateStun();
				if (physics != nullptr && !physics->frozen)
					attacker.attack.update(timeDelta, physics->getPos(), direction->dir);

				for (CombatComponent& defender : EntitySystem::GetPool<CombatComponent>()) {
					attackCheck(attacker, defender);
				}
			}
			else {
				//if we aren't the attacker, only update our defense
				CombatComponent& defender = *EntitySystem::GetComp<CombatComponent>(id);
				attackCheck(attacker, defender);
			}
		}
	}
}

void CombatSystem::attackCheck(CombatComponent& attacker, CombatComponent& defender) {
	if (defender.getId() != attacker.getId()) {
		EntityId attackerId = attacker.getId();
		EntityId defenderId = defender.getId();
		if (!hitList.contains(attackerId)) {
			hitList.add(attackerId, std::vector<EntityId>{});
		}

		//who we've hit
		std::vector<EntityId>& hitIds = hitList[attackerId];

		//if there has been a change in the attack state (started attacking, changed attack etc)
		if (attacker.attack.pollAttackChange()) {
			size_t priorSize = hitIds.size();
			hitIds.clear();
			hitIds.reserve(priorSize);
		}
		if (attacker.teamId != defender.teamId) {
			if (attacker.getActiveHitbox() != nullptr) {
				const AABB& hitbox = attacker.getActiveHitbox()->hit;
				if (hitbox.intersects(defender.getBoundingBox())) {
					auto hurtboxes = defender.hurtboxes;
					for (auto& hurtbox : hurtboxes) {
						if (hurtbox.box.intersects(hitbox)) {
							//who we've already hit
							if (std::find(hitIds.begin(), hitIds.end(), defenderId) == hitIds.end()) {
								defender.damage(attacker.rollDamage());
								defender.stun(attacker.getStun());
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
