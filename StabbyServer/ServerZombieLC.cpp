#include "pch.h"
#include "ServerZombieLC.h"
#include "PhysicsComponent.h"

#define MAX_PREV_HITBOXES 20

ServerZombieLC::ServerZombieLC(EntityId id_) :
	ZombieLC{id_}
{}

void ServerZombieLC::runLogic() {
	//ZombieLC::runLogic();


	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);

	const Vec2f & pos = physics->getPos();

	if (zombieState.facing == 1)
		hurtbox.pos = pos + Vec2f{ -5.0f, -20.0f };
	else if (zombieState.facing == -1)
		hurtbox.pos = pos + Vec2f{ -8.0f, -20.0f };

	if (prevHurtboxes.size() == MAX_PREV_HITBOXES)
		prevHurtboxes.pop_front();
	prevHurtboxes.push_back(hurtbox);
}

const AABB * ServerZombieLC::getHurtboxes(int * size) const {
	*size = 1;

	/*
	int offset = 4;

	if (prevHurtboxes.size() > offset)
		return &prevHurtboxes[prevHurtboxes.size() - offset];
	else if (!prevHurtboxes.empty())
		return &prevHurtboxes[prevHurtboxes.size() - 1];
	else
		return &hurtbox;
		*/
	return &hurtbox;
}

void ServerZombieLC::die() {
	EntitySystem::FreeComps<ServerZombieLC>(1, &id);
	EntitySystem::FreeComps<PhysicsComponent>(1, &id);
	EntitySystem::FreeComps<PositionComponent>(1, &id);
}
