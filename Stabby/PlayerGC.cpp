#include "pch.h"
#include "PlayerGC.h"
#include "ClientPlayerLC.h"
#include "FileNotFoundException.h"
#include <iostream>
#include "PhysicsAABB.h"
#include "HeadParticleLC.h"



PlayerGC::PlayerGC(EntityId id_) :
	id{ id_ },
	offset{0, 0},
	shouldSpawnHead{true}
{}

void PlayerGC::load() {

	AnimatedSprite & sprite = static_cast<AnimatedSprite &>(EntitySystem::GetComp<RenderComponent>(id)->getSprite());

	//keep in mind graphics gale starts at frame 1, not 0
	sprite.addAnimation(idle, 0, 1);
	sprite.addAnimation(walking, 1, 17);
	sprite.addAnimation(slash1, 17, 20);
	sprite.addAnimation(slash2, 20, 24);
	sprite.addAnimation(slash3, 24, 30);
	sprite.addAnimation(roll, 30, 38);
	sprite.addAnimation(stun, 38, 39);
	sprite.addAnimation(dead, 39, 51);

	//ClientPlayerLC* playerLogic = EntitySystem::GetComp<ClientPlayerLC>(id);
	//Vec2f res = playerLogic->getRes();
	Vec2f res = { 4.0f, 20.0f };
	offset = Vec2f{ (sprite.getObjRes().abs().x - res.x) / 2, (sprite.getObjRes().abs().y - res.y) };
}

EntityId PlayerGC::getId() const {
	return id;
}

void PlayerGC::spawnHead(Vec2f pos, std::string filePath) {
	unsigned int id_;
	EntitySystem::GenEntities(1, &id_);
	EntitySystem::MakeComps<HeadParticleLC>(1, &id_);
	EntitySystem::MakeComps<RenderComponent>(1, &id_);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id_);

	HeadParticleLC * particle = EntitySystem::GetComp<HeadParticleLC>(id_);
	particle->setLife(240);

	PhysicsComponent * physics = EntitySystem::GetComp<PhysicsComponent>(id_);
	physics->collider = AABB{ pos, {4, 4} };
	physics->weight = 3;
	physics->vel = { 0, -80 };

	RenderComponent * image = EntitySystem::GetComp<RenderComponent>(id_);
	image->loadSprite<Sprite>(filePath);
}
