#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "Connection.h"
#include "PositionComponent.h"
#include "PlayerLC.h"
#include "ServerPlayerComponent.h"

//user stores their connection, id, as well as the logic component
class User {
public:
	User(NetworkId id_, ConnectionPtr&& con);

	EntityId getId() const;
	NetworkId getNetId() const;
	Connection * getConnection();
	PlayerLC & getPlayer();
	ServerPlayerComponent& getServerPlayer();
	PhysicsComponent & getPhysics();
	PositionComponent & getPosition();
	PlayerStateComponent & getPlayerState();
	CombatComponent & getCombat();

private:
	EntityId id;
	NetworkId netId;
	ConnectionPtr con;
};