#include "stdafx.h"
#include "AnimatedSprite.h"

AnimatedSprite::AnimatedSprite(const std::string &filePath, Vec2f objRes_,
	int columns_, int totalSteps_,
	Vec2f pos_, Vec2f origin_, Vec2f scale_, Vec2f imgOffset_) :

	Sprite{ filePath, pos_, origin_, scale_, imgOffset_},
	columns{columns_},
	totalSteps{ totalSteps_ },

	currentFrame{ 0 },
	currentStep{ 0 },
	looping{false},
	currentAnimationId{0}
{
	data.objRes = objRes_;
}

void AnimatedSprite::forward() {
	if (currentFrame < currentAnimation.x)
		currentFrame = currentAnimation.x;
	currentStep++;
	if (currentStep == totalSteps) {
		currentStep = 0;
		currentFrame++;
		if (currentFrame >= currentAnimation.y) {
			if (looping)
				currentFrame = currentAnimation.x;
			else
				currentFrame = currentAnimation.y - 1;
		}
	}
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::backward() {
	if (currentFrame > currentAnimation.y - 1)
		currentFrame = currentAnimation.y - 1;
	currentStep++;
	if (currentStep == totalSteps) {
		currentStep = 0;
		currentFrame--;
		if (currentFrame <= currentAnimation.x - 1) {
			if (looping)
				currentFrame = currentAnimation.y - 1;
			else
				currentFrame = currentAnimation.x;
		}
	}
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

int AnimatedSprite::getFrame() {
	return currentFrame;
}

void AnimatedSprite::setFrame(int frame) {
	currentFrame = frame;
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::addAnimation(int id, int beginFrame, int endFrame) {
	animations[id] = { beginFrame, endFrame };
}

void AnimatedSprite::setAnimation(int id) {
	currentAnimationId = id;
	currentAnimation = animations[id];
	currentFrame = currentAnimation.x;
	currentStep = 0;
	data.imgOffset = { (currentFrame % columns) * abs(data.objRes.x), (currentFrame / columns) * abs(data.objRes.y) };
}

void AnimatedSprite::resetDelay() {
	currentStep = totalSteps - 1;
}

Vec2i AnimatedSprite::getAnimation(int id) {
	return animations[id];
}

int AnimatedSprite::getCurrentAnimationId() {
	return currentAnimationId;
}

Sprite * AnimatedSprite::clone() const {
	return new AnimatedSprite(*this);
}
