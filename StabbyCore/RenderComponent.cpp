#include "stdafx.h"
#include "RenderComponent.h"
#include "PositionComponent.h"

RenderComponent::RenderComponent(EntityId id_) :
	id{id_},
	offset{0, 0}
{
	if (!EntitySystem::Contains<PositionComponent>() || EntitySystem::GetComp<PositionComponent>(id) == nullptr) {
		EntitySystem::MakeComps<PositionComponent>(1, &id);
	}
}

RenderComponent::RenderComponent(const RenderComponent & other) :
	id{other.id},
	offset{other.offset}
{
	if (other.sprite != nullptr) {
		sprite = std::unique_ptr<Sprite>{ other.sprite->clone() };
	}
	else {
		sprite = nullptr;
	}
}

RenderComponent & RenderComponent::operator=(const RenderComponent & other) {
	id = other.id;
	offset = other.offset;
	if (other.sprite != nullptr) {
		sprite = std::unique_ptr<Sprite>{ other.sprite->clone() };
	}
	else {
		sprite = nullptr;
	}
	return *this;
}

Sprite * RenderComponent::getSprite() {
	return sprite.get();
}

Vec2f RenderComponent::getImgRes() const {
	return sprite->getImgRes();
}

void RenderComponent::setObjRes(Vec2f objRes) {
	sprite->setObjRes(objRes);
}

void RenderComponent::setScale(Vec2f scale) {
	sprite->setScale(scale);
}

unsigned int RenderComponent::getRenderBufferId() const {
	return sprite->getRenderBufferId();
}

const ImgData & RenderComponent::getImgData() const {
	return sprite->getImgData();
}

EntityId RenderComponent::getId() const {
	return id;
}
