#pragma once
#include "EntitySystem.h"
#include "Vec2.h"
#include "NetworkTypes.h"

enum class State {
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
			health == other.health && stunFrame == other.stunFrame && facing == other.facing;
	}


	bool operator!=(const PlayerState & other) {
		return !(*this == other);
	}

	Time_t when;

	//goverened DIRECTLY by the associtaed playerLC
	State state;
	int rollFrame;
	unsigned int activeAttack;
	unsigned int attackFrame;
	int health;
	int stunFrame;
	int facing;

	//governed (updated by us) by the associated physics and position components
	Vec2f pos;
	Vec2f vel;
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
private:
	EntityId id;
	PlayerState playerState;
};