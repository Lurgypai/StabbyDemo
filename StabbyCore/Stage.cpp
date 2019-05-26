#include "stdafx.h"
#include "Stage.h"
#include "AABB.h"
#include "RenderComponent.h"

Stage::Stage() :
	pos{ -(STAGE_WIDTH / 2), 0 },
	colliders{}
{
	colliders.emplace_back(Vec2f{ -(STAGE_WIDTH / 2), 0 }, Vec2f{STAGE_WIDTH, STAGE_HEIGHT});
	EntitySystem::GenEntities(1, &id);
}

const std::vector<AABB> & Stage::getColliders() const {
	return colliders;
}

void Stage::loadGraphics(std::string filePath) {
	EntitySystem::MakeComps<RenderComponent>(1, &id);
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);

	render->loadSprite<Sprite>(filePath, pos);
	render->getSprite().setScale({STAGE_WIDTH / render->getSprite().getImgRes().x, STAGE_HEIGHT / render->getSprite().getImgRes().y });
}

EntityId Stage::getId() {
	return id;
}
