#include "pch.h"
#include "User.h"
#include "PhysicsAABB.h"
#include "PlayerStateComponent.h"

User::User(NetworkId id_, ConnectionPtr && con_) :
	netId{id_},
	con{std::move(con_)}
{
	EntitySystem::GenEntities(1, &id);
	EntitySystem::MakeComps<PlayerLC>(1, &id);
	EntitySystem::MakeComps<ServerPlayerComponent>(1, &id);
	EntitySystem::MakeComps<PhysicsComponent>(1, &id);
	EntitySystem::MakeComps<CombatComponent>(1, &id);
	EntitySystem::GetComp<PhysicsComponent>(id)->setRes({ -2, -20 });
	EntitySystem::GetComp<PhysicsComponent>(id)->setRes(Vec2f{ static_cast<float>(PlayerLC::PLAYER_WIDTH), static_cast<float>(PlayerLC::PLAYER_HEIGHT) });
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

PlayerLC & User::getPlayer() {
	return *EntitySystem::GetComp<PlayerLC>(id);
}

ServerPlayerComponent& User::getServerPlayer() {
	return *EntitySystem::GetComp<ServerPlayerComponent>(id);
}

PhysicsComponent & User::getPhysics() {
	return *EntitySystem::GetComp<PhysicsComponent>(id);
}

PositionComponent & User::getPosition() {
	return *EntitySystem::GetComp<PositionComponent>(id);
}

PlayerStateComponent & User::getPlayerState() {
	return *EntitySystem::GetComp<PlayerStateComponent>(id);
}

CombatComponent& User::getCombat() {
	return *EntitySystem::GetComp<CombatComponent>(id);
}
