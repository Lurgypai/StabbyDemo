#include "pch.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "PhysicsAABB.h"
#include "HeadParticleLC.h"
#include "PositionComponent.h"
#include "DirectionComponent.h"


PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	shouldSpawnHead{true},
	prevXVel{0},
	prevAttack{0}
{
	if (!EntitySystem::Contains<RenderComponent>() || EntitySystem::GetComp<RenderComponent>(id) == nullptr) {
		EntitySystem::MakeComps<RenderComponent>(1, &id);
	}
}

void PlayerGC::loadAnimations() {
	RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*render->getSprite());

	//keep in mind graphics gale starts at frame 1, not 0
	animSprite.addAnimation(idle, 0, 1);
	animSprite.addAnimation(walking, 1, 17);
	animSprite.addAnimation(slash1, 17, 20);
	animSprite.addAnimation(slash2, 20, 24);
	animSprite.addAnimation(slash3, 24, 30);
	animSprite.addAnimation(roll, 30, 39);
	animSprite.addAnimation(stun, 39, 40);
	animSprite.addAnimation(dead, 40, 52);

	//ClientPlayerLC* playerLogic = EntitySystem::GetComp<ClientPlayerLC>(id);
	//Vec2f res = playerLogic->getRes();
	Vec2f res = { 4.0f, 20.0f };
	render->offset = Vec2f{ (animSprite.getObjRes().abs().x - res.x) / 2, (animSprite.getObjRes().abs().y - res.y) };

	animSprite.setAnimation(idle);
	defaultFrameDelay = animSprite.frameDelay;
}

void PlayerGC::spawnHead(Vec2f pos) {
	unsigned int id_;
	EntitySystem::GenEntities(1, &id_);
	EntitySystem::MakeComps<HeadParticleLC>(1, &id_);
	EntitySystem::MakeComps<RenderComponent>(1, &id_);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id_);

	HeadParticleLC * particle = EntitySystem::GetComp<HeadParticleLC>(id_);
	particle->setLife(240);

	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id_);
	physics->setRes({4, 4});
	physics->weight = 3;
	physics->vel = { 0, -80 };

	PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id_);
	position->pos = pos;

	RenderComponent * image = EntitySystem::GetComp<RenderComponent>(id_);
	image->loadSprite<Sprite>("images/head.png");
}

void PlayerGC::updateState(double timeDelta) {
	PlayerStateComponent * player = EntitySystem::GetComp<PlayerStateComponent>(id);
	if (player != nullptr) {
		RenderComponent * render = EntitySystem::GetComp<RenderComponent>(id);
		DirectionComponent * direction = EntitySystem::GetComp<DirectionComponent>(id);
		AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*render->getSprite());
		PlayerState state = player->playerState;

		int width = animSprite.getObjRes().abs().x;
		int height = animSprite.getObjRes().abs().y;
		animSprite.setObjRes(Vec2i{ direction->dir * width, height });

		State plrState = state.state;
		static bool shouldSpawnParticles = false;
		
		if (state.attackFreezeFrame == 0) {
			shouldSpawnParticles = true;
			if (state.state != prevState) {
				prevState = state.state;
				animSprite.looping = true;
				switch (plrState) {
				case State::dead:
					animSprite.frameDelay = defaultFrameDelay;
					animSprite.setAnimation(dead);
					animSprite.looping = false;
					break;
				case State::stunned:
					animSprite.frameDelay = defaultFrameDelay;
					animSprite.setAnimation(stun);
					break;
				case State::rolling:
					animSprite.frameDelay = defaultFrameDelay;
					animSprite.setAnimation(roll);
					animSprite.looping = false;
					break;
				}
			}

			if (prevAttack != state.activeAttack) {
				prevAttack = state.activeAttack;
				if (state.state == State::attacking) {
					animSprite.frameDelay = defaultFrameDelay / state.attackSpeed;
					if (state.activeAttack == 1) {
						animSprite.setAnimation(slash1);
					}
					else if (state.activeAttack == 2)
						animSprite.setAnimation(slash2);
					else if (state.activeAttack == 3)
						animSprite.setAnimation(slash3);
					animSprite.looping = false;
				}
			}

			if (state.state == State::free) {
				animSprite.frameDelay = defaultFrameDelay / state.moveSpeed;
				shouldSpawnHead = true;
				if (state.vel.x == 0 && animSprite.getCurrentAnimationId() != idle) {
					animSprite.setAnimation(idle);
				}
				else if (state.vel.x != 0 && animSprite.getCurrentAnimationId() != walking) {
					animSprite.setAnimation(walking);
				}
				prevXVel = state.vel.x;
			}

			animSprite.forward(timeDelta);

			//put this at the end so we don't modify the RenderComponent pool and screw up the sprite reference
			if (shouldSpawnHead && plrState == State::dead) {
				shouldSpawnHead = false;
				PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
				spawnHead(position->pos);
			}
		}
		else if (shouldSpawnParticles) {
			shouldSpawnParticles = false;
			float offset = 7;
			Vec2f spawnPos = state.pos;
			spawnPos.x += offset * direction->dir;
			spawnPos.y -= 15;
			if (state.activeAttack == 1) {
				Particle p1{ spawnPos, -90 + 36.0f * direction->dir, 1.6f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 5, p1, 2.0f, 0.0f, 0.0f, { 0.3f, 3.0f });
			}
			else if (state.activeAttack == 2) {
				Particle p1{ spawnPos, -90 + 35.0f * direction->dir, 2.0f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 7, p1, 23.0f, 0.0f, 0.0f, { 0.3f, 3.0f });
			}
			else if (state.activeAttack == 3) {
				Particle p1{ spawnPos, -90 + 53.0f * direction->dir, 2.4f, 150, 0 };
				GLRenderer::SpawnParticles("blood", 15, p1, 35.0f, 0.0f, 0.0f, { 0.5f, 1.0f });
			}
		}
	}
}

EntityId PlayerGC::getId() const {
	return id;
}
