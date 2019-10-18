#include "stdafx.h"

#include "RandomUtil.h"

#include "CombatComponent.h"
#include "DirectionComponent.h"
#include "PositionComponent.h"

CombatComponent::CombatComponent(EntityId id_) :
	id{id_},
	invulnerable{false},
	stats{}
{
	if (id != 0) {
		if (!EntitySystem::Contains<DirectionComponent>() || EntitySystem::GetComp<DirectionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<DirectionComponent>(1, &id);
			DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
			direction->dir = -1;
		}
	}
}

EntityId CombatComponent::getId() const {
	return id;
}

AABB CombatComponent::getBoundingBox() {
	Vec2f pos{}, res{};
	Vec2f min{ hurtboxes[0].box.pos }, max{ hurtboxes[0].box.pos + hurtboxes[0].box.res };
	for (auto & box : hurtboxes) {
		auto & hurtbox = box.box;
		min.x = hurtbox.pos.x < min.x ? hurtbox.pos.x : min.x;
		min.y = hurtbox.pos.y < min.x ? hurtbox.pos.y : min.y;
		max.x = hurtbox.pos.x + hurtbox.res.x > max.x ? hurtbox.pos.x + hurtbox.res.x : max.x;
		max.y = hurtbox.pos.y + hurtbox.res.y > max.y ? hurtbox.pos.y + hurtbox.res.y : max.y;
	}

	pos = min;
	res = max - min;

	return AABB(pos, res);
}

void CombatComponent::updateHurtboxes() {
	DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

	for (auto& hurtbox : hurtboxes) {
		if (direction->dir > -1) {
			hurtbox.box.pos = position->pos + hurtbox.offset;
		} else {
			hurtbox.box.pos.y = position->pos.y + hurtbox.offset.y;
			hurtbox.box.pos.x = position->pos.x + hurtbox.offset.x - hurtbox.box.res.x;
		}
	}
}

void CombatComponent::onAttackLand()
{
}

void CombatComponent::onKill()
{
}

void CombatComponent::onDeath()
{
}

void CombatComponent::onHeal(int amount)
{
}

void CombatComponent::onDamage(int amount)
{
}

const Hitbox * CombatComponent::getActiveHitbox() const {
	return attack.getActive();
}

void CombatComponent::damage(int i) {
	if (!invulnerable)
		health -= i;
	onDamage(i);
}

void CombatComponent::heal(int i) {
	health += i;
	onHeal(i);
}

int CombatComponent::rollDamage() {
	float doCrit = randFloat(0.0f, 1.0f);
	int guaranteedCrits = static_cast<int>(stats.critChance);
	float critChance = stats.critChance - guaranteedCrits;

	//calculate the guaranteed crits, if the leftover chance happens do it, otherwise return the base damage.
	int totalDamage =  (stats.baseDamage * stats.critMultiplier) * guaranteedCrits + (doCrit < critChance ? stats.baseDamage * stats.critMultiplier : stats.baseDamage);

	float doVampire = randFloat(0.0f, 1.0f);
	if (doVampire < stats.vampirismChance) {
		heal(totalDamage * stats.vampirismMultiplier);
	}

	return totalDamage;
}
