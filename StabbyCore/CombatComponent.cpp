#include "stdafx.h"

#include "RandomUtil.h"

#include "CombatComponent.h"
#include "DirectionComponent.h"
#include "PositionComponent.h"
#include <iostream>

CombatComponent::CombatComponent(EntityId id_) :
	id{id_},
	invulnerable{false},
	stats{},
	stunFrame{0}
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
		hurtbox.box.pos = position->pos + hurtbox.offset;
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

void CombatComponent::onHeal(unsigned int amount)
{
}

void CombatComponent::onDamage(unsigned int amount)
{
}

void CombatComponent::onStun(unsigned int amount) {
}

const Hitbox * CombatComponent::getActiveHitbox() const {
	return attack.getActive();
}

unsigned int CombatComponent::getStun() {
	auto activeAttack = attack.getActive();
	return activeAttack == nullptr ? 0 : activeAttack->stun;
}

bool CombatComponent::isStunned() {
	return stunFrame != 0;
}

void CombatComponent::updateStun() {
	if (stunFrame > 0)
		--stunFrame;
	else
		stunFrame = 0;
}

void CombatComponent::damage(unsigned int i) {
	if (!invulnerable)
		health -= i;
	onDamage(i);
}

void CombatComponent::heal(unsigned int i) {
	health += i;
	onHeal(i);
}

void CombatComponent::stun(unsigned int i) {
	if(!invulnerable)
		stunFrame = i;
	onStun(i);
}

bool CombatComponent::hasHitEntity(const EntityId& target) {
	return hitEntities.size() > target && hitEntities[target];
}

void CombatComponent::addHitEntity(EntityId hit) {
	if (hitEntities.size() < hit + 1)
		hitEntities.resize(hit + 1);
	hitEntities.set(hit, true);
}

void CombatComponent::clearHitEntities() {
	hitEntities.zero();
}

int CombatComponent::rollDamage() {

	const Hitbox* active = attack.getActive();

	if (active != nullptr) {

		int activeId = attack.getActiveId();

		float doCrit = randFloat(0.0f, 1.0f);
		int guaranteedCrits = static_cast<int>(stats.critChance);
		float critChance = stats.critChance - guaranteedCrits;

		//calculate the guaranteed crits, if the leftover chance happens do it, otherwise return the base damage.
		int totalDamage = (active->damage * stats.baseDamage * stats.critMultiplier) * guaranteedCrits + (doCrit < critChance ? active->damage * stats.baseDamage * stats.critMultiplier : active->damage * stats.baseDamage);

		float doVampire = randFloat(0.0f, 1.0f);
		if (doVampire < stats.vampirismChance) {
			heal(totalDamage * stats.vampirismMultiplier);
		}

		return totalDamage;
	}

	return 0;
}

EntityId CombatComponent::getLastAttacker() {
	return lastAttacker;
}