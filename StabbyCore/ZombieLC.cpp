#include "stdafx.h"
#include "ZombieLC.h"
#include "PhysicsComponent.h"
#include "EntityBaseComponent.h"

ZombieLC::ZombieLC(EntityId id_) :
	id{ id_ },
	moveSpeed{ 50 },
	relaxFramMax{ 100 },
	stunFrameMax{ 50 },
	chargeFrameMax{70},
	deathFrameMax{5},
	idleFrameMax{60},
	targetId{0}
{
	zombieState = ZombieState();
	zombieState.health = 85;
	zombieState.facing = 1;
	
	if (id != 0) {
		if (!EntitySystem::Contains<PhysicsComponent>() || EntitySystem::GetComp<PhysicsComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
			physics->weight = 3;
			physics->setRes(Vec2f{ static_cast<float>(ZombieLC::WIDTH), static_cast<float>(ZombieLC::HEIGHT) });
		}
		if (!EntitySystem::Contains<CombatComponent>() || EntitySystem::GetComp<CombatComponent>(id) == nullptr) {
			EntitySystem::MakeComps<PhysicsComponent>(1, &id);
			CombatComponent * combat = EntitySystem::GetComp<CombatComponent>(id);
			
		}
	}
}

void ZombieLC::runLogic() {
	PositionComponent * targetPos = EntitySystem::GetComp<PositionComponent>(targetId);
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	CombatComponent* combat = EntitySystem::GetComp<CombatComponent>(id);

	const Vec2f & pos = physics->getPos();

	switch (zombieState.state) {
	case State::walking:
		if (zombieState.relaxFrame == relaxFramMax) {
			zombieState.relaxFrame = 0;
			if (targetPos != nullptr) {
				float distance = targetPos->pos.distance(pos);
				if (distance < 250) {
					zombieState.facing = 0;
					if (targetPos->pos.x < pos.x)
						zombieState.facing = -1;
					else if (targetPos->pos.x > pos.x)
						zombieState.facing = 1;

					if (distance < 80) {
						zombieState.relaxFrame = 0;
						zombieState.chargeFrame = 0;
						zombieState.state = State::charging;
					}
					else {
						physics->accelerate({ zombieState.facing * moveSpeed, 0 });
					}
				}
				else {
					targetId = 0;
					zombieState.state = State::idle;
				}
			}
		}
		else {
			++zombieState.relaxFrame;
			if (physics->vel.x > 0)
				physics->accelerate({ -1, 0 });
			else if (physics->vel.x < 0)
				physics->accelerate({ 1, 0 });
		}
		break;
	case State::charging:
		if (zombieState.chargeFrame != chargeFrameMax) {
			++zombieState.chargeFrame;
		}
		else {
			physics->accelerate((75 * zombieState.facing) - 90, 300);
			zombieState.state = State::attacking;
			zombieState.attackChanged = true;
			//combat->attack.startAttacking();
		}
		break;
	case State::attacking:
		if (physics->grounded) {
			zombieState.state = State::walking;
			physics->vel.x = 0;
		}
		break;
	case State::stunned:
		if (zombieState.stunFrame != stunFrameMax)
			++zombieState.stunFrame;
		else
			zombieState.state = State::walking;
		break;
	case State::dead:
		if (zombieState.deathFrame != deathFrameMax)
			++zombieState.deathFrame;
		else
			EntitySystem::GetComp<EntityBaseComponent>(id)->isDead = true;
	case State::idle:
		if (zombieState.idleFrame != idleFrameMax) {
			++zombieState.idleFrame;
		}
		else {
			zombieState.idleFrame = 0;
			zombieState.state = State::walking;
		}
	}

	/*
	if(zombieState.facing == 1)
		hitbox.pos = pos + Vec2f{ -1.0f, -14.0f };
	else if(zombieState.facing == -1)
		hitbox.pos = pos + Vec2f{ -10.0f, -14.0f };
	*/

	zombieState.pos = pos;
	zombieState.vel = physics->vel;
}

bool ZombieLC::isStunned() const {
	return zombieState.state == State::stunned;
}

void ZombieLC::setState(const ZombieState & state) {
	zombieState = state;
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	physics->teleport(state.pos);
	physics->vel = state.vel;
}

ZombieLC::ZombieState ZombieLC::getState() {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	zombieState.pos = physics->getPos();
	zombieState.vel = physics->vel;
	return zombieState;
}
