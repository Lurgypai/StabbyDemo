#include "stdafx.h"
#include "PlayerLC.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"
#include "DebugIO.h"
#include "ClimbableComponent.h"

#include <iostream>

PlayerLC::PlayerLC(EntityId id_) :
	id{id_},
	jumpSpeed{ 120 },
	prevButton2{false},
	prevButton3{ false },
	isBeingHit{false},
	rollVel{220},
	storedVel{0},
	rollFrameMax{68},
	deathFrame{0},
	deathFrameMax{200},
	attackFreezeFrameMax{17},
	healFrameMax{60},
	healDelayMax{120},
	horizontalAccel{10.0},
	stepDistance{50},
	climbDistance{35}
{
	//do not default construct
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || EntitySystem::GetComp<PhysicsComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->weight = 3;
			physics->setRes(Vec2f{ static_cast<float>(PlayerLC::PLAYER_WIDTH), static_cast<float>(PlayerLC::PLAYER_HEIGHT) });

			PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
			position->pos = { -2, -20 };
		}
		if (!EntitySystem::Contains<PlayerStateComponent>() || EntitySystem::GetComp<PlayerStateComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PlayerStateComponent>(1, &id);
			PlayerStateComponent * stateComp = EntitySystem::GetComp<PlayerStateComponent>(id);
			stateComp->playerState.health = 100;
			stateComp->playerState.facing = 1;
			stateComp->playerState.activeAttack = 0;
			stateComp->playerState.moveSpeed = 1.0;
			stateComp->playerState.attackSpeed = 1.0;
		}
		if (!EntitySystem::Contains<DirectionComponent>() || EntitySystem::GetComp<DirectionComponent>(id) == nullptr) {
			EntitySystem::MakeComps<DirectionComponent>(1, &id);
			DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
			direction->dir = -1;
		}
		if (!EntitySystem::Contains<CombatComponent>() || EntitySystem::GetComp<CombatComponent>(id) == nullptr) {
			EntitySystem::MakeComps<CombatComponent>(1, &id);
		}
	}
}

void PlayerLC::update(double timeDelta, const Controller & controller) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
	CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);

	PlayerState& state = playerState->playerState;
	Attack & attack = combat->attack;
	


	bool attackToggledDown{ false };
	bool currButton2 = controller[ControllerBits::BUTTON_2];
	if (prevButton2 != currButton2) {
		if (currButton2) {
			attackToggledDown = true;
			if (state.state == State::attacking && !attack.getNextIsBuffered()) {
				attack.bufferNext();
				attackChange = true;
			}
		}
	}

	Vec2f & vel = comp->vel;
	comp->weightless = false;

	if (!comp->frozen) {
		switch (state.state) {
		case State::stunned:
			if (combat->isStunned()) {
				vel.x = 0;
			}
			else {
				state.state = State::free;
			}
			break;
		case State::attacking:
			vel.x = 0;
			if (attack.getActiveId() == 0)
				state.state = State::free;

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

				vel.x = (rollVel * state.moveSpeed * direction->dir) + (dir * 80 * state.moveSpeed);
			}
			else {
				combat->invulnerable = false;
				state.rollFrame = 0;
				vel.x = storedVel;
				state.state = State::free;
			}
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
		case State::healing:
		{
			vel.x = 0;
			if (state.healDelay == healDelayMax) {
				state.healFrame++;
				if (state.healFrame == healFrameMax) {
					state.healFrame = 0;
					combat->heal(2);
				}

				bool currButton1 = controller[ControllerBits::BUTTON_1];
				if (currButton1 != prevButton1 && currButton1) {
					state.state = State::free;
				}
			}
			else {
				++state.healDelay;
			}
			break;
		}
		case State::climbing:

			bool overlaps{false};
			for (auto& climbable : EntitySystem::GetPool<ClimbableComponent>()) {
				if (comp->intersects(climbable.collider)) {
					overlaps = true;
					break;
				}
			}

			comp->weightless = true;

			if (!overlaps) {
				state.state = State::free;
				break;
			}

			Vec2f dir{ 0, 0 };
			if (controller[ControllerBits::LEFT]) {
				--dir.x;
			}
			if (controller[ControllerBits::RIGHT]) {
				++dir.x;
			}
			if (controller[ControllerBits::UP]) {
				--dir.y;
			}
			if (controller[ControllerBits::DOWN]) {
				++dir.y;
			}

			vel = dir * static_cast<float>(climbDistance * state.moveSpeed);
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
	}

	if (combat->health <= 0)
		state.state = State::dead;

	else if (combat->isStunned())
		state.state = State::stunned;

	state.pos = comp->getPos();
	state.vel = comp->vel;
	state.facing = direction->dir;
	state.health = combat->health;
	state.stunFrame = combat->stunFrame;

	state.activeAttack = attack.getActiveId();
	state.attackFrame = attack.getCurrFrame();

	prevButton1 = controller[ControllerBits::BUTTON_1];
	prevButton2 = controller[ControllerBits::BUTTON_2];
	prevButton3 = controller[ControllerBits::BUTTON_3];

	DebugIO::setLine(6, std::to_string(state.health));
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

void PlayerLC::respawn() {
	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PlayerState & state = playerState->playerState;
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);

	combat->health = 100;
	combat->stunFrame = 0;

	state.state = State::free;
	state.activeAttack = 0;
	state.attackFrame = 0;

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);\
	Attack & attack = combat->attack;

	position->pos = Vec2f{static_cast<float>( -PLAYER_WIDTH / 2), static_cast<float>(-PLAYER_HEIGHT) };
	comp->vel = { 0, 0 };
	attack.setActive(0);
	attack.setFrame(0);
}

EntityId PlayerLC::getId() const {
	return id;
}

void PlayerLC::free(const Controller & controller, bool attackToggledDown_) {

	PhysicsComponent * comp = EntitySystem::GetComp<PhysicsComponent>(id);
	Vec2f & vel = comp->vel;

	PlayerStateComponent * playerState = EntitySystem::GetComp<PlayerStateComponent>(id);
	PlayerState & state = playerState->playerState;
	DirectionComponent* direction = EntitySystem::GetComp<DirectionComponent>(id);

	CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);
	Attack & attack = combat->attack;

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

	//try to start rolling
	if (state.state != State::attacking) {
		bool currButton3 = controller[ControllerBits::BUTTON_3];
		if (prevButton3 != currButton3) {
			if (currButton3) {
				state.state = State::rolling;
				combat->invulnerable = true;
				storedVel = vel.x;
				vel.x = direction->dir * rollVel;
			}
		}
	}

	if (EntitySystem::Contains<ClimbableComponent>()) {
		for (auto& climable : EntitySystem::GetPool<ClimbableComponent>()) {
			if (comp->intersects(climable.collider) && controller[ControllerBits::UP]) {
				state.state = State::climbing;
				comp->weightless = true;
			}
		}
	}

	bool currButton1 = controller[ControllerBits::BUTTON_1];
	if (currButton1 != prevButton1 && currButton1) {
		state.state = State::healing;
		state.healDelay = 0;
		state.healFrame = 0;
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
		if (dir != 0)
			direction->dir = dir;

		float targetVel = dir * state.moveSpeed * stepDistance;
		float accel = state.moveSpeed * horizontalAccel;
		if (vel.x < targetVel - accel) {
			vel.x += accel;
		}
		else if (vel.x > targetVel + accel) {
			vel.x -= accel;
		}
		else {
			vel.x = targetVel;
		}
	}
}
