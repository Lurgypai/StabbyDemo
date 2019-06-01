#pragma once
#include "EntitySystem.h"
#include "Sprite.h"
#include "FileNotFoundException.h"
#include "GLRenderer.h"

#include <iostream>

using SpritePtr = std::unique_ptr<Sprite>;

class RenderComponent {
public:
	RenderComponent(EntityId id_ = 0);
	RenderComponent(const RenderComponent & other);
	
	RenderComponent & operator=(const RenderComponent & other);
	
	template<typename T, typename... Args>
	void loadSprite(Args... args);

	template<typename T, typename U>
	void setSprite(U&& u);

	virtual void updatePosition();

	Vec2f getImgRes() const;

	void setScale(Vec2f scale);

	unsigned int getRenderBufferId() const;
	const ImgData & getImgData() const;

	EntityId getId() const;
protected:
	SpritePtr sprite;
	EntityId id;
};

template<typename T, typename ...Args>
inline void RenderComponent::loadSprite(Args ...args) {
	try {
		sprite = std::make_unique<T>(args...);
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}
}

template<typename T, typename U>
inline void RenderComponent::setSprite(U && u) {
	try {
	sprite = std::make_unique<T>(std::forward(u));
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}
}
