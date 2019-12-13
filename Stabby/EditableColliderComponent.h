#pragma once
#include "AABB.h"
#include "Stage.h"

enum class AnchorSide {
	right,
	left,
	top,
	bottom,
	topright,
	bottomright,
	topleft,
	bottomleft
};

enum class EditableState {
	moving,
	resizing,
	idle
};

class EditableColliderComponent {

	friend class EditableSystem;

public:
	EditableColliderComponent(EntityId id_ = 0);

	const EntityId getId() const;
	void update(int camId);

	const AABB& getCollider() const;
	StageElement getType() const;
private:
	static constexpr int minSize = 2;

	void adjustLeft(const Vec2f mousePos);
	void adjustRight(const Vec2f mousePos);
	void adjustTop(const Vec2f mousePos);
	void adjustBottom(const Vec2f mousePos);

	EntityId id;

	StageElement type;

	Vec2i anchorPoint;
	Vec2f prevPoint;
	Vec2f prevRes;
	AABB collider;
	EditableState state;
	AnchorSide anchorSide;

	bool prevButton1;
	bool prevButton2;
	bool prevButton3;
};