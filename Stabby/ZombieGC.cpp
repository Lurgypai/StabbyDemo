#include "pch.h"
#include "ZombieGC.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"

ZombieGC::ZombieGC(EntityId id_) :
	RenderComponent{ id_ },
	prevXVel{0},
	center{ 0, 0 }
{}

void ZombieGC::loadAnimations() {
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*sprite);

	//keep in mind graphics gale starts at frame 1, not 0
	animSprite.addAnimation(0, 0, 11);
	animSprite.setAnimation(0);

	Vec2f res = { 4.0f, 20.0f };
	offset = Vec2f{ (sprite->getObjRes().abs().x - res.x) / 2, (sprite->getObjRes().abs().y - res.y) };
}

void ZombieGC::updatePosition() {
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*sprite);

	ZombieLC * zombie = EntitySystem::GetComp<ZombieLC>(id);

	if (position != nullptr && physics != nullptr && zombie != nullptr) {
		center = physics->center();

		int width = animSprite.getObjRes().abs().x;
		int height = animSprite.getObjRes().abs().y;

		animSprite.setObjRes(Vec2i{ zombie->getState().facing * width, height });

		animSprite.setPos(position->pos - offset);

		if (!zombie->isStunned()) {
			animSprite.looping = false;

			if (physics->vel.x != prevXVel) {
				if (physics->vel.x != 0 && prevXVel == 0) {
					animSprite.setAnimation(0);
				}
				prevXVel = physics->vel.x;
			}

			animSprite.forward();
		}
		else if (zombie->getState().state == ZombieLC::State::dead) {
			animSprite.setObjRes(Vec2i{ 0, 0 });
			Vec2f spawnPos = center;

			Particle p1{ spawnPos, -90, 1.7f, 100, 0 };
			GLRenderer::SpawnParticles("blood", 20, p1, 90.0f, 0.0f, 0, { 2.0f, 10.0f });
		}
	}
	else {
		EntitySystem::FreeComps<ZombieGC>(1, &id);
	}
}
