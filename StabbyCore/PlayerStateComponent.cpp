#include "stdafx.h"
#include "PlayerStateComponent.h"
#include "PositionComponent.h"
#include "PhysicsComponent.h"

PlayerStateComponent::PlayerStateComponent(EntityId id_) :
	id{id_}
{
	playerState = PlayerState();
}

EntityId PlayerStateComponent::getId() const {
	return id;
}

PlayerState PlayerStateComponent::getPlayerState() const {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);

	//construct a new one using the data we already have, and the missing data
	PlayerState copy = playerState;
	
	if (physics != nullptr) {
		copy.vel = physics->vel;
		copy.pos = physics->getPos();
	}

	return copy;
}

void PlayerStateComponent::setPlayerState(const PlayerState & state_) {
	playerState = state_;
}

void PlayerStateComponent::setWhen(Time_t when) {
	playerState.when = when;
}

void PlayerStateComponent::setState(State state_) {
	playerState.state = state_;
}

void PlayerStateComponent::setRollFrame(int rollFrame) {
	playerState.rollFrame = rollFrame;
}

void PlayerStateComponent::setActiveAttack(unsigned int activeAttack) {
	playerState.activeAttack = activeAttack;
}

void PlayerStateComponent::setAttackFrame(unsigned int attackFrame) {
	playerState.attackFrame = attackFrame;
}

void PlayerStateComponent::setHealth(int health) {
	playerState.health = health;
}

void PlayerStateComponent::setStunFrame(int stunFrame) {
	playerState.stunFrame = stunFrame;
}

void PlayerStateComponent::setFacing(int facing) {
	playerState.facing = facing;
}

void PlayerStateComponent::setSpawnPoint(const Vec2f & spawnPoint_) {
	playerState.spawnPoint = spawnPoint_;
}

void PlayerStateComponent::setAttackFreezeFrame(int attackFreezeFrame_) {
	playerState.attackFreezeFrame = attackFreezeFrame_;
}

void PlayerStateComponent::setAttackSpeed(double attackSpeed) {
	playerState.attackSpeed = attackSpeed;
}

void PlayerStateComponent::setMoveSpeed(double moveSpeed) {
	playerState.moveSpeed = moveSpeed;
}
