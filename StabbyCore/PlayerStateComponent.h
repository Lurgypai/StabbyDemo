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
	dead
};

struct PlayerState {

	bool operator==(const PlayerState & other) {
		return when == other.when && pos == other.pos && vel == other.vel && state == other.state &&
			rollFrame == other.rollFrame && activeAttack == other.activeAttack && attackFrame == other.attackFrame &&
			health == other.health && stunFrame == other.stunFrame && facing == other.facing &&
			spawnPoint == other.spawnPoint && attackFreezeFrame == other.attackFreezeFrame && frozen == other.frozen &&
			attackSpeed == other.attackSpeed && moveSpeed == other.moveSpeed;
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
public:
	PlayerStateComponent(EntityId id = 0);
	EntityId getId() const;

	PlayerState getPlayerState() const;
	void setPlayerState(const PlayerState & state_);
	void setWhen(Time_t when);
	void setState(State state_);
	void setRollFrame(int rollFrame);
	void setActiveAttack(unsigned int activeAttack);
	void setAttackFrame(unsigned int attackFrame);
	void setHealth(int health);
	void setStunFrame(int stunFrame);
	void setFacing(int facing);
	void setSpawnPoint(const Vec2f & spawnPoint_);
	void setAttackFreezeFrame(int attackFreezeFrame_);
	void setAttackSpeed(double attackSpeed);
	void setMoveSpeed(double moveSpeed);
private:
	EntityId id;
	PlayerState playerState;
};