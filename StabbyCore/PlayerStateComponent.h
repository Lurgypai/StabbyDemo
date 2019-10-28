#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "NetworkTypes.h"
#include <cstdint>

enum class State : uint8_t {
	free,
	attacking,
	rolling,
	stunned,
	dead,
	climbing,
	healing
};

struct PlayerState {
	bool operator==(const PlayerState & other) {
		return when == other.when && pos == other.pos && vel == other.vel && state == other.state &&
			rollFrame == other.rollFrame && activeAttack == other.activeAttack && attackFrame == other.attackFrame &&
			health == other.health && stunFrame == other.stunFrame && facing == other.facing &&
			spawnPoint == other.spawnPoint && attackFreezeFrame == other.attackFreezeFrame && frozen == other.frozen &&
			attackSpeed == other.attackSpeed && moveSpeed == other.moveSpeed && healFrame == other.healFrame && healDelay == other.healDelay;
	}


	bool operator!=(const PlayerState & other) {
		return !(*this == other);
	}

	Time_t when;

	State state;
	uint32_t rollFrame;
	uint32_t activeAttack;
	uint32_t attackFrame;
	int32_t health;
	uint32_t stunFrame;
	uint32_t healFrame;
	uint32_t healDelay;
	int32_t facing;
	Vec2f spawnPoint;
	int32_t attackFreezeFrame;
	double attackSpeed;
	double moveSpeed;

	Vec2f pos;
	Vec2f vel;
	bool frozen;
};

class PlayerStateComponent {
	//only player controllers should have control over the state. Do not use anything else, it will muddy the design.

public:
	PlayerStateComponent(EntityId id = 0);
	EntityId getId() const;

	PlayerState playerState;
	std::string attackId;
private:
	EntityId id;
};