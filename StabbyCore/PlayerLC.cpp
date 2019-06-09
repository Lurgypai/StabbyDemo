#include "stdafx.h"
#include "PlayerLC.h"
#include "PositionComponent.h"

#include <iostream>

PlayerLC::PlayerLC(EntityId id_) :
	CombatComponent{id_},
	maxXVel{ 50 },
	xAccel{ 10 },
	jumpSpeed{ 120 },
	attack{},
	prevButton2{false},
	prevButton3{ false },
	isBeingHit{false},
	rollVel{220},
	storedVel{0},
	rollFrameMax{68},
	stunFrameMax{80},
	stunSlideSpeed{10},
	deathFrame{0},
	deathFrameMax{200},
	attackFreezeFrameMax{17}
{
	if (!EntitySystem::Contains<PhysicsComponent>() || EntitySystem::GetComp<PhysicsComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PhysicsComponent>(1, &id);
		PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
		physics->weight = 3;
		physics->setRes(Vec2f{ static_cast<float>(PlayerLC::PLAYER_WIDTH), static_cast<float>(PlayerLC::PLAYER_HEIGHT) });

		PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
		position->pos = {-2, -20};
	}
	if (!EntitySystem::Contains<PlayerStateComponent>() || EntitySystem::GetComp<PlayerStateComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PlayerStateComponent>(1, &id);
		PlayerStateComponent * stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
		stateComp->setHealth(100);
		stateComp->setFacing(1);
	}
}

void PlayerLC::update(double timeDelta, const Controller & controller) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);

	PlayerState state = playerState->getPlayerState();

	state.frozen = comp->frozen;

	bool attackToggledDown{ false };
	bool currButton2 = controller[ControllerBits::BUTTON_2];
	if (prevButton2 != currButton2) {
		prevButton2 = currButton2;
		if (currButton2) {
			attackToggledDown = true;
			if (state.state == State::attacking && !attack.getNextIsBuffered()) {
				attack.bufferNext();
				attackChange = true;
			}
		}
	}

	attack.setActive(state.activeAttack);
	attack.setFrame(state.attackFrame);

	Vec2f & vel = comp->vel;

	//the problem is, we're frozen client side, but not serverside. Add frozen to the playerstate
	if (!comp->frozen) {

		//always update the attack. Hitboxes should only be out when we are stuck in attack mode.
		attack.update(position->pos, comp->getRes(), state.facing);

		switch (state.state) {
		case State::stunned:
			if (state.stunFrame == 0) {
				vel.x = 0;
				++state.stunFrame;
			}
			else if (state.stunFrame != stunFrameMax) {
				++state.stunFrame;
			}
			else {
				state.stunFrame = 0;
				state.state = State::free;
			}
			playerState->setPlayerState(state);
			break;
		case State::attacking:
			vel.x = 0;
			if (attack.getActiveId() == 0)
				state.state = State::free;

			state.activeAttack = attack.getActiveId();
			state.attackFrame = attack.getCurrFrame();
			playerState->setPlayerState(state);
			break;
		case State::rolling:
			if (state.rollFrame != rollFrameMax) {
				state.rollFrame++;

				int dir{ 0 };
				if (controller[ControllerBits::LEFT]) {
					--dir;
				}
				if (controller[ControllerBits::RIGHT]) {
					++dir;
				}

				vel.x = (rollVel * state.facing) + (dir * 80);
			}
			else {
				state.rollFrame = 0;
				vel.x = storedVel;
				state.state = State::free;
			}
			playerState->setPlayerState(state);
			break;
		case State::free:
			free(controller, attackToggledDown);
			break;
		case State::dead:
			++deathFrame;

			if (deathFrame == deathFrameMax) {
				deathFrame = 0;
				respawn();
			}
			break;
		}

		if (position->pos.y > 1000) {
			respawn();
		}
	}
	else {
		++state.attackFreezeFrame;
		if (state.attackFreezeFrame == attackFreezeFrameMax) {
			comp->frozen = false;
			state.attackFreezeFrame = 0;
		}
		playerState->setPlayerState(state);
	}

	state.pos = comp->getPos();
	state.vel = comp->vel;
}

PhysicsComponent * PlayerLC::getPhysics() {
	return EntitySystem::GetComp<PhysicsComponent>(id);
}

Vec2f PlayerLC::getVel() const {
	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	return comp->vel;
}

Vec2f PlayerLC::getRes() const {
	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	return comp->getRes();
}

Attack & PlayerLC::getAttack() {
	return attack;
}

int PlayerLC::getActiveId() {
	return attack.getActiveId();
}

void PlayerLC::damage(int amount) {
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PlayerState state = playerState->getPlayerState();

	if (state.state != State::rolling) {

		state.health -= amount;
		state.state = State::stunned;

		playerState->setPlayerState(state);

		if (state.health <= 0)
			die();
	}
}

void PlayerLC::onAttackLand() {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->frozen = true;
}

void PlayerLC::die() {
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PlayerState state = playerState->getPlayerState();

	state.state = State::dead;
	state.rollFrame = 0;
	state.stunFrame = 0;
	state.activeAttack = 0;
	state.attackFrame = 0;
	attack.setActive(0);
	attack.setFrame(0);
	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	comp->vel = { 0, 0 };

	playerState->setPlayerState(state);
}



AABB * PlayerLC::getActiveHitbox() {
	Hitbox * activeHitbox = attack.getActive();
	if (activeHitbox != nullptr)
		return &activeHitbox->hit;
	else
		return nullptr;
}

int PlayerLC::getActiveDamage() {
	return 15;
}

bool PlayerLC::readAttackChange() {
	bool a = attackChange;
	attackChange = false;
	return a;
}

const AABB * PlayerLC::getHurtboxes(int * size ) const {
	if (size != nullptr)
		*size = 1;
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	return &physics->getCollider();
}

void PlayerLC::respawn() {
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerState state = playerState->getPlayerState();

	state.state = State::free;
	state.health = 100;
	state.activeAttack = 0;
	state.attackFrame = 0;

	playerState->setPlayerState(state);
	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	position->pos = Vec2f{static_cast<float>( -PLAYER_WIDTH / 2), static_cast<float>(-PLAYER_HEIGHT) };
	comp->vel = { 0, 0 };
	attack.setActive(0);
	attack.setFrame(0);
}

void PlayerLC::free(const Controller & controller, bool attackToggledDown_) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	Vec2f & vel = comp->vel;

	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PlayerState state = playerState->getPlayerState();

	if (attackToggledDown_) {
		if (attack.canStartAttacking()) {
			state.state = State::attacking;
			if (attack.getActiveId() == 0) {
				attack.startAttacking();
				attackChange = true;
				state.activeAttack = attack.getActiveId();
				state.attackFrame = attack.getCurrFrame();
			}
		}
	}

	//try to start attack
	if (state.state != State::attacking) {
		bool currButton3 = controller[ControllerBits::BUTTON_3];
		if (prevButton3 != currButton3) {
			prevButton3 = currButton3;
			if (currButton3) {
				state.state = State::rolling;
				storedVel = vel.x;
				vel.x = state.facing * rollVel;
			}
		}
	}

	if (state.state == State::free) {
		int dir{ 0 };
		if (controller[ControllerBits::LEFT]) {
			--dir;
		}
		if (controller[ControllerBits::RIGHT]) {
			++dir;
		}
		if (controller[ControllerBits::UP]) {
			if (comp->grounded) {
				vel.y = -jumpSpeed;
			}
		}
		//otherwise do
		if (dir == 0) {
			if (vel.x > 0)
				vel.x -= xAccel;
			else if (vel.x < 0)
				vel.x += xAccel;
		}
		else {
			state.facing = dir;
			vel.x += (dir * xAccel);
		}

		if (vel.x > maxXVel)
			vel.x = maxXVel;
		else if (vel.x < -maxXVel)
			vel.x = -maxXVel;
	}

	playerState->setPlayerState(state);
}
