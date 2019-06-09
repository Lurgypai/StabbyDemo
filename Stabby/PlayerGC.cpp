#include "pch.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "PhysicsAABB.h"
#include "HeadParticleLC.h"
#include "PositionComponent.h"



PlayerGC::PlayerGC(EntityId id_) :
	RenderComponent{ id_ },
	offset{0, 0},
	shouldSpawnHead{true},
	prevXVel{0},
	prevAttack{0}
{}

void PlayerGC::loadAnimations() {
	AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*sprite);

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
	offset = Vec2f{ (sprite->getObjRes().abs().x - res.x) / 2, (sprite->getObjRes().abs().y - res.y) };
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

void PlayerGC::updatePosition() {
	PlayerStateComponent * player = EntitySystem::GetComp<PlayerStateComponent>(id);
	if (player != nullptr) {
		PositionComponent * position = EntitySystem::GetComp<PositionComponent>(id);
		AnimatedSprite & animSprite = static_cast<AnimatedSprite &>(*sprite);
		PlayerState state = player->getPlayerState();

		int width = animSprite.getObjRes().abs().x;
		int height = animSprite.getObjRes().abs().y;
		animSprite.setObjRes(Vec2i{ state.facing * width, height });

		animSprite.setPos(position->pos - offset);

		State plrState = state.state;
		static bool shouldSpawnParticles = false;
		if (state.attackFreezeFrame == 0) {
			shouldSpawnParticles = true;
			if (state.state != prevState) {
				prevState = state.state;
				animSprite.looping = true;
				switch (plrState) {
				case State::dead:
					animSprite.setAnimation(dead);
					animSprite.looping = false;
					break;
				case State::stunned:
					animSprite.setAnimation(stun);
					break;
				case State::rolling:
					animSprite.setAnimation(roll);
					animSprite.looping = false;
					break;
				}
			}

			if (prevAttack != state.activeAttack) {
				prevAttack = state.activeAttack;
				if (state.state == State::attacking) {
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
				shouldSpawnHead = true;
				if (state.vel.x == 0 && animSprite.getCurrentAnimationId() != idle) {
					animSprite.setAnimation(idle);
				}
				else if (state.vel.x != 0 && animSprite.getCurrentAnimationId() != walking){
					animSprite.setAnimation(walking);
				}
				prevXVel = state.vel.x;
			}
			
			animSprite.forward();

			//put this at the end so we don't modify the RenderComponent pool and screw up the sprite reference
			if (shouldSpawnHead && plrState == State::dead) {
				shouldSpawnHead = false;
				spawnHead(state.pos);
			}
		}
		else if (shouldSpawnParticles){
			shouldSpawnParticles = false;
			float offset = 7;
			Vec2f spawnPos = state.pos;
			spawnPos.x += offset * state.facing;
			spawnPos.y -= 15;
			if (state.activeAttack == 1) {
				Particle p1{ spawnPos, -90 + 36.0f * state.facing, 1.6f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 5, p1, 2.0f, 0.0f, 0.0f, { 0.3f, 3.0f });
			}
			else if (state.activeAttack == 2) {
				Particle p1{ spawnPos, -90 + 35.0f * state.facing, 2.0f, 100, 0 };
				GLRenderer::SpawnParticles("blood", 7, p1, 23.0f, 0.0f, 0.0f, { 0.3f, 3.0f });
			}
			else if (state.activeAttack == 3) {
				Particle p1{ spawnPos, -90 + 53.0f * state.facing, 2.4f, 150, 0 };
				GLRenderer::SpawnParticles("blood", 15, p1, 35.0f, 0.0f, 0.0f, { 0.5f, 1.0f });

			}
		}
	}
}
