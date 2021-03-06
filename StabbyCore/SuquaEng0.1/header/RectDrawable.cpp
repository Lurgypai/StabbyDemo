#include "stdafx.h"
#include "RectDrawable.h"
#include "GLRenderer.h"

void RectDrawable::draw() {
	GLRenderer::DrawPrimitves({shape}, r, g, b);
}

void RectDrawable::setPos(Vec2f pos) {
	shape.pos = pos;
}

IDrawable* RectDrawable::clone() {
	return new RectDrawable(*this);
}
