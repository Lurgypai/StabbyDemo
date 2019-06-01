#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"

class RenderSystem {
public:
	template<typename T>
	void draw();
};

template<typename T>
inline void RenderSystem::draw() {
	Pool<T> & pool = EntitySystem::GetPool<T>();
	for (auto & comp : pool) {
		comp.updatePosition();
		GLRenderer::SetBuffer(comp.getRenderBufferId());
		GLRenderer::Buffer(comp.getImgData());
	}
}
