#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "Connection.h"
#include "ServerPlayerLC.h"
#include "PositionComponent.h"

//user stores their connection, id, as well as the logic component
class User {
public:
	User(NetworkId id_, ConnectionPtr&& con);

	EntityId getId() const;
	NetworkId getNetId() const;
	Connection * getConnection();
	ServerPlayerLC & getPlayer();
	PhysicsComponent & getPhysics();
	PositionComponent & getPosition();
	PlayerStateComponent & getPlayerState();

private:
	EntityId id;
	NetworkId netId;
	ConnectionPtr con;
};