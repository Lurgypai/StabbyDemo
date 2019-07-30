#pragma once
#include "AABB.h"
#include <array>

//begin and end framess
struct Hitbox {
	AABB hit;
	Vec2f offset;
	unsigned int startup;
	unsigned int active;
	unsigned int ending;
};

class Attack {
public:
	Attack();
	void setActive(int i);
	void setFrame(int frame);
	void setSpeed(double newSpeed);
	bool canStartAttacking();
	bool getNextIsBuffered() const;
	void startAttacking();
	Hitbox* getActive();
	unsigned int getActiveId();
	unsigned int getCurrFrame();
	unsigned int getCurrentTotalFrames();
	void bufferNext();
	void update(double timeDelta, Vec2f pos, Vec2f res, int facing);
private:
	std::array<Hitbox, 3> hitboxes;
	int active = 0;
	//current frame relative to the current attack
	unsigned int currFrame;
	bool nextIsBuffered;
	//delay for attacking again
	int restartDelay;
	int restartDelayMax;
	//delay between each frame of attacking
	double frameDelay;
	double elapsedTime;
	double speed;
};