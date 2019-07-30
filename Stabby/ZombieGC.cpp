#include "pch.h"
#include "ZombieGC.h"
#include "AnimatedSprite.h"
#include "PhysicsComponent.h"

ZombieGC::ZombieGC(EntityId id_) :
	id{ id_ },
	prevXVel{0},
	center{ 0, 0 }
{
	if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
		EntitySystem::MakeComps<RenderComponent>(1, &id);
	}
}

void ZombieGC::loadAnimations() {
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*render->getSprite());

	//keep in mind graphics gale starts at frame 1, not 0
	animSprite.addAnimation(0, 0, 11);
	animSprite.addAnimation(1, 12, 23);
	animSprite.setAnimation(1);

	Vec2f res = { 4.0f, 20.0f };
	render->offset = Vec2f{ (animSprite.getObjRes().abs().x - res.x) / 2, (animSprite.getObjRes().abs().y - res.y) };
}

void ZombieGC::updateState(double timeDelta) {
	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id);
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*render->getSprite());

	ZombieLC * zombie = EntitySystem::GetComp<ZombieLC>(id);

	if (physics != nullptr && zombie != nullptr) {
		center = physics->center();

		int width = animSprite.getObjRes().abs().x;
		int height = animSprite.getObjRes().abs().y;

		animSprite.setObjRes(Vec2i{ zombie->getState().facing * width, height });

		if (!zombie->isStunned()) {
			if (zombie->getState().state != ZombieLC::State::dead && zombie->getState().state != ZombieLC::State::idle) {
				animSprite.looping = false;

				if (physics->vel.x != prevXVel) {
					if (physics->vel.x != 0 && prevXVel == 0) {
						animSprite.setAnimation(0);
					}
					prevXVel = physics->vel.x;
				}

				animSprite.forward(timeDelta);
			}
			else if (zombie->getState().state == ZombieLC::State::idle) {
				animSprite.looping = false;
				if (animSprite.getCurrentAnimationId() != 1) {
					animSprite.setAnimation(1);
				}
				animSprite.forward(timeDelta);
			}
			else if (zombie->getState().state == ZombieLC::State::dead) {
				animSprite.setObjRes(Vec2i{ 0, 0 });
				Vec2f spawnPos = center;

				Particle p1{ spawnPos, -90, 1.7f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 20, p1, 90.0f, 0.0f, 0, { 2.0f, 10.0f });
			}
		}
	}
}

EntityId ZombieGC::getId() const {
	return id;
}
