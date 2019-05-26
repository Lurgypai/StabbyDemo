#include "pch.h"
#include "User.h"
#include "PhysicsAABB.h"
#include "PlayerData.h"

User::User(NetworkId id_, ConnectionPtr && con_) :
	netId{id_},
	con{std::move(con_)}
{
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<ServerPlayerLC>(1, &id);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);
	EntitySystem::GetComp<PhysicsComponent>(id)->collider = AABB{ {-2, -20}, Vec2f{static_cast<float>(PlayerLC::PLAYER_WIDTH), static_cast<float>(PlayerLC::PLAYER_HEIGHT)} };
	EntitySystem::GetComp<PhysicsComponent>(id)->weight = 3;
}

EntityId User::getId() const {
	return id;
}

NetworkId User::getNetId() const {
	return netId;
}

Connection * User::getConnection() {
	return con.get();
}

ServerPlayerLC & User::getPlayer() {
	return *EntitySystem::GetComp<ServerPlayerLC>(id);
}

PhysicsComponent & User::getPhysics() {
	return *EntitySystem::GetComp<PhysicsComponent>(id);
}
