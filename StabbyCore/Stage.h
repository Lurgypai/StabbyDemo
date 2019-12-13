#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "AABB.h"
#include <string>

enum class StageElement {
	collideable,
	climbable
};

class Stage {
public:
	static const std::string folder;

	Stage();
	Stage(const std::string & stage);
	Vec2f getSpawnPos() const;
	void loadGraphics();
	const std::vector<EntityId> & getCollideables() const;
	const std::vector<EntityId>& getClimbables() const;
	const std::vector<EntityId>& getRenderables() const;

private:

	std::string name;
	std::vector<EntityId> collideables;
	std::vector<EntityId> climbables;
	std::vector<EntityId> renderables;
};