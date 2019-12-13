#include "pch.h"
#include "SpawnZombieCommand.h"
#include "ZombieLC.h"
#include "ZombieGC.h"
#include "PhysicsComponent.h"
#include "AnimatedSprite.h"

std::string SpawnZombieCommand::getTag() const {
	return "spawnzombie";
}

void SpawnZombieCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 3) {
		try {
			float x = std::stof(args[1]);
			float y = std::stof(args[2]);

			const int count{ 1 };
			EntityId id[count];
			EntitySystem::GenEntities(count, &id[0]);
			EntitySystem::MakeComps<ZombieLC>(count, &id[0]);
			EntitySystem::MakeComps<ZombieGC>(count, &id[0]);

			AnimatedSprite sprite{ "images/zambo.png", Vec2i{32, 32} };

			for (int i = 0; i != count; ++i) {
				EntitySystem::GetComp<PhysicsComponent>(id[i])->teleport({ x, y });
				EntitySystem::GetComp<RenderComponent>(id[i])->setDrawable<AnimatedSprite>(sprite);
				EntitySystem::GetComp<ZombieGC>(id[i])->loadAnimations();
			}
		}
		catch (std::invalid_argument e) {
		}
	}
}
