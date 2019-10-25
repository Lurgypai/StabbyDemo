#pragma once
#include "EntitySystem.h"
#include "Sprite.h"
#include "FileNotFoundException.h"
#include "GLRenderer.h"

#include <iostream>

using SpritePtr = std::unique_ptr<Sprite>;

class RenderComponent {
public:
	Vec2f offset;

	RenderComponent(EntityId id_ = 0);
	RenderComponent(const RenderComponent & other);
	
	RenderComponent & operator=(const RenderComponent & other);
	
	template<typename T, typename... Args>
	void loadSprite(Args... args);

	template<typename T, typename U>
	void setSprite(U&& u);
	
	template<typename U>
	void setSprite(std::unique_ptr<U> u);

	Sprite * getSprite();

	Vec2f getImgRes() const;
	void setObjRes(Vec2f objRes);

	void setScale(Vec2f scale);

	unsigned int getRenderBufferId() const;
	const ImgData & getImgData() const;

	EntityId getId() const;
protected:
	SpritePtr sprite;
	EntityId id;

	friend class RenderSystem;
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
	sprite = std::make_unique<T>(std::forward<U>(u));
	}
	catch (FileNotFoundException e) {
		std::cout << e.what() << '\n';
	}
}
