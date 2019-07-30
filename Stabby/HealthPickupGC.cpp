#include "pch.h"
#include "HealthPickupGC.h"
#include "HealthPickupLC.h"
#include "PositionComponent.h"
#include "EntityBaseComponent.h"
#include "RandomUtil.h"

HealthPickupGC::HealthPickupGC(EntityId id_) :
	id{id_}
{
	if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
		EntitySystem::MakeComps<RenderComponent>(1, &id);
	}
}

void HealthPickupGC::updateState(double timeDelta) {
	if (EntitySystem::GetComp<EntityBaseComponent>(id)->isDead) {
		RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
		Vec2f center = render->getSprite()->getPos() + Vec2f{ 2.5, 2.5 };

		std::string tag = "blood";
		float vel = 2.5f;
		int amount = EntitySystem::GetComp<HealthPickupLC>(id)->amount / 10;

		while (amount-- > 0) {
			float angle = randFloat(-20.0f, 20.0f);

			for (int i = 0; i != 6; ++i) {
				Particle p1{ center + Vec2f{static_cast<float>(i % 3), static_cast<float>(i / 3)}, angle - 90.0f, vel, 500, 0 };
				GLRenderer::SpawnParticles(tag, 1, p1);
			}

			Particle p1{ center + Vec2f{-1, 0}, angle - 90.0f, vel, 500, 0 };
			GLRenderer::SpawnParticles(tag, 1, p1);
			p1.pos = center + Vec2f{ 3, 0 };
			GLRenderer::SpawnParticles(tag, 1, p1);
			p1.pos = center + Vec2f{ 1, 2 };
			GLRenderer::SpawnParticles(tag, 1, p1);
			p1.pos = center + Vec2f{ 0, -1 };
			GLRenderer::SpawnParticles(tag, 1, p1);
			p1.pos = center + Vec2f{ 2, -1 };
			GLRenderer::SpawnParticles(tag, 1, p1);
		}
	}
}

EntityId HealthPickupGC::getId() const {
	return id;
}
