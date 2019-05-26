#include "stdafx.h"
#include "RenderSystem.h"
#include "EntitySystem.h"
#include "RenderComponent.h"

void RenderSystem::drawAll() {
	for (auto& comp : EntitySystem::GetPool<RenderComponent>()) {
		GLRenderer::SetBuffer(comp.getRenderBufferId());
		GLRenderer::Buffer(comp.getImgData());
	}
}
