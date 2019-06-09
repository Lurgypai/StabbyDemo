#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "AABB.h"
#include <string>

#define STAGE_WIDTH 1000
#define STAGE_HEIGHT 10

class Stage {
public:
	Stage();
	Vec2f getSpawnPos() const;
	void loadGraphics(std::string filePath);
	const std::vector<AABB>& getColliders() const;
	EntityId getId();

private:
	std::vector<AABB> colliders;
	Vec2f pos;
	EntityId id;
};