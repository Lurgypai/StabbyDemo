#include "stdafx.h"
#include "Stage.h"
#include "RenderComponent.h"
#include "PositionComponent.h"
#include "ClimbableComponent.h"

Stage::Stage() :
	pos{ -(STAGE_WIDTH / 2), 0 },
	colliders{}
{
	colliders.emplace_back(Vec2f{ -(STAGE_WIDTH / 2), 0 }, Vec2f{STAGE_WIDTH, STAGE_HEIGHT});
	EntitySystem::GenEntities(1, &id);

	EntitySystem::GenEntities(1, &ladder);
	EntitySystem::MakeComps<ClimbableComponent>(1, &ladder);
	EntitySystem::GetComp<ClimbableComponent>(ladder)->collider = { {0.0f, 0.0f}, {16.0f, 128.0f} };
	EntitySystem::GetComp<PositionComponent>(ladder)->pos = { -200, -128 };
}

Vec2f Stage::getSpawnPos() const {
	return Vec2f{0, 0};
}

const std::vector<AABB> & Stage::getColliders() const {
	return colliders;
}

void Stage::loadGraphics(std::string filePath) {
	EntitySystem::MakeComps<RenderComponent>(1, &id);
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);

	EntitySystem::MakeComps<RenderComponent>(1, &ladder);
	EntitySystem::GetComp<RenderComponent>(ladder)->loadSprite<Sprite>("images/ladder.png");

	position->pos = pos;

	render->loadSprite<Sprite>(filePath, pos);
	render->setScale({STAGE_WIDTH / render->getImgRes().x, STAGE_HEIGHT / render->getImgRes().y });
}

EntityId Stage::getId() {
	return id;
}
