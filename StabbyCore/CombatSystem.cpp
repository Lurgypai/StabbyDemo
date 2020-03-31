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
	EntityId attackerId = attacker.getId();
	EntityId defenderId = defender.getId();
	if (attackerId != defenderId) {
		if (attacker.teamId != 0 && defender.teamId != 0 && attacker.teamId != defender.teamId) {

			bool attackChanged = attacker.attack.pollAttackChange();
			if (attackChanged)
				attacker.clearHitEntities();

			if (attackChanged || !attacker.hasHitEntity(defenderId)) {
				auto* activeHitbox = attacker.getActiveHitbox();
				if (activeHitbox != nullptr) {
					const AABB& hit = activeHitbox->hit;
					if (hit.intersects(defender.getBoundingBox())) {
						auto hurtboxes = defender.hurtboxes;
						for (auto& hurtbox : hurtboxes) {
							if (hurtbox.box.intersects(hit)) {
								defender.damage(attacker.rollDamage());
								defender.stun(attacker.getStun());
								defender.lastAttacker = attackerId;
								attacker.onAttackLand();

								attacker.addHitEntity(defenderId);
							}
						}
					}
				}
			}
		}
	}
}
