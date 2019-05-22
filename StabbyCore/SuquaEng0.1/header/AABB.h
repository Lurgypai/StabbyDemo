#pragma once
#include "Vec2.h"
#include <unordered_map>
#include "EntitySystem.h"

//create an AABB class that holds velocity, and handles moving bounding boxes, including collision.

class AABB {
public:
	enum class Direction {
		up,
		down,
		left,
		right
	};

	AABB(Vec2f pos_, Vec2f res_);
	Vec2f getPos() const;
	Vec2f getRes() const;
	Vec2f center() const;

	void center(const Vec2f & newCenter);
	void setPos(const Vec2f& other);
	void setRes(const Vec2f& res_);

	bool contains(Vec2f point);
	bool intersects(const AABB& other) const;

protected:
	Vec2f pos;
	Vec2f res;
};