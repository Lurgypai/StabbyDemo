#include "stdafx.h"
#include "Attack.h"

#include <iostream>

Attack::Attack() :
	restartDelayMax{ 0 },
	frameDelay{ 0 },
	id{ "" },

	attackChanged{ false },
	elapsedTime{ 0.0 },

	currFrame{ 0 },
	nextIsBuffered{ false },
	active{ 0 },
	restartDelay{ 0 },
	speed{ 0.0 }
{}

Attack::Attack(const std::string& id_, int restartDelayMax_, double frameDelay_) :
	restartDelayMax{ restartDelayMax_ },
	frameDelay{ frameDelay_ },
	id{ id_ },

	attackChanged{ false },
	elapsedTime{ 0.0 },

	currFrame{ 0 },
	nextIsBuffered{ false },
	active{ 0 },
	restartDelay{ 0 },
	speed{ 1.0 }
{}

void Attack::setActive(int i) {
	if (i >= 0 && i < hitboxes.size())
		active = i;
}

void Attack::setFrame(int frame) {
	currFrame = frame;
}

void Attack::setSpeed(double newSpeed) {
	speed = newSpeed;
}

bool Attack::canStartAttacking() {
	return restartDelay == restartDelayMax;
}

bool Attack::getNextIsBuffered() const {
	return nextIsBuffered;
}

void Attack::startAttacking() {
	if (restartDelay == restartDelayMax) {
		active = 1;
		attackChanged = true;
	}
}

const Hitbox * Attack::getActive() const {
	if (active != 0 && active <= hitboxes.size()) {
		const Hitbox * hitbox = &hitboxes[active - 1];
		if (hitbox->startup < currFrame && currFrame < hitbox->startup + hitbox->active) {
			return hitbox;
		}
	}
	return nullptr;
}

unsigned int Attack::getActiveId(){
	return active;
}

unsigned int Attack::getCurrFrame() {
	return currFrame;
}

unsigned int Attack::getCurrentTotalFrames() {
	return hitboxes[active - 1].startup + hitboxes[active - 1].active + hitboxes[active - 1].ending;
}

unsigned int Attack::getDamage() const {
	return hitboxes[active - 1].damage;
}

double Attack::getSpeed() {
	return speed;
}

void Attack::bufferNext() {
	nextIsBuffered = true;
}

void Attack::update(double timeDelta, Vec2f pos, int facing) {
	if (restartDelay != restartDelayMax) {
		++restartDelay;
		return;
	}
	//update pos
	if (active != 0) {
		Hitbox& hbox = hitboxes[active - 1];
		Vec2f offset = hbox.offset;
		if(facing == -1)
			offset.x = -(offset.x + hbox.hit.res.x);

		hbox.hit.pos = (pos + offset);
	}

	//if a hitbox is running, keep moving through the frames. 
	if (active != 0 && currFrame < hitboxes[active - 1].startup + hitboxes[active - 1].active + hitboxes[active - 1].ending) {
		if (nextIsBuffered && currFrame > hitboxes[active - 1].startup + hitboxes[active - 1].active && active < hitboxes.size()) {
			++active;
			currFrame = 0;
			elapsedTime = 0;
			nextIsBuffered = false;
			attackChanged = true;
		}
		else {
			elapsedTime += timeDelta;
			int passedFrames = static_cast<int>(elapsedTime / (frameDelay / speed));
			if (passedFrames != 0)
				elapsedTime = 0;

			currFrame += passedFrames;
		}
	}
	//otherwise reset (only if we weren't attacking)
	else if(active != 0) {
		active = 0;
		currFrame = 0;
		elapsedTime = 0;
		nextIsBuffered = false;
		restartDelay = 0;
	}
}

void Attack::addHitbox(Hitbox && hitbox) {
	hitboxes.emplace_back(hitbox);
}

bool Attack::pollAttackChange() {
	if (attackChanged) {
		attackChanged = false;
		return true;
	}
	return false;
}

std::string Attack::getId() {
	return id;
}
