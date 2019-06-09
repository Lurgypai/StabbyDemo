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
	if (EntitySystem::Contains<T>()) {
		Pool<T> & pool = EntitySystem::GetPool<T>();
		unsigned int prevRenderBufferId{ 0 };
		for (auto & comp : pool) {

			comp.updatePosition();

			unsigned int renderBufferId = comp.getRenderBufferId();
			if (prevRenderBufferId != renderBufferId) {
				prevRenderBufferId = renderBufferId;
				GLRenderer::SetBuffer(renderBufferId);

			}

			GLRenderer::Buffer(comp.getImgData());
		}
	}
}
