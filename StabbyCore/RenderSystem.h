#pragma once
#include "GLRenderer.h"
#include "RenderComponent.h"]
#include "PositionComponent.h"

class RenderSystem {
public:
	void drawAll();
};

void RenderSystem::drawAll() {
	if (EntitySystem::Contains<RenderComponent>()) {
		unsigned int prevRenderBufferId{0};
		bool setBuffer{true};
		for (auto & comp : EntitySystem::GetPool<RenderComponent>()) {
			Sprite * sprite = comp.sprite.get();
			if (sprite != nullptr) {
				PositionComponent * position = EntitySystem::GetComp<PositionComponent>(comp.getId());
				sprite->setPos(position->pos - comp.offset);

				unsigned int renderBufferId = comp.getRenderBufferId();
				if (setBuffer || prevRenderBufferId != renderBufferId) {
					prevRenderBufferId = renderBufferId;
					GLRenderer::SetBuffer(renderBufferId);
					setBuffer = false;
				}

				GLRenderer::Buffer(comp.getImgData());
			}
		}
	}
}