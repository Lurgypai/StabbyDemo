#include "stdafx.h"
#include "RenderComponent.h"

RenderComponent::RenderComponent(EntityId id_) :
	id{id_}
{}

RenderComponent::RenderComponent(const RenderComponent & other) :
	id{other.id}
{
	if (other.sprite != nullptr) {
		sprite = std::unique_ptr<Sprite>{ other.sprite->clone() };
	}
}

RenderComponent & RenderComponent::operator=(const RenderComponent & other) {
	id = other.id;
	if (other.sprite != nullptr) {
		sprite = std::unique_ptr<Sprite>{ other.sprite->clone() };
	}
	return *this;
}

Sprite & RenderComponent::getSprite() {
	return *sprite;
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
