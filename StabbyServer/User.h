#pragma once
#include "Vec2.h"
#include "NetworkTypes.h"
#include "Connection.h"
#include "PositionComponent.h"
#include "PlayerLC.h"
#include "ServerPlayerComponent.h"
#include "OnlineComponent.h"
#include "Host.h"

//user stores their connection, id, as well as the logic component
class User {
public:
	User(NetworkId id_, ConnectionPtr&& con);

	EntityId getId() const;
	PeerId getPeerId() const;
	Connection * getConnection();
	PlayerLC & getPlayer();
	ServerPlayerComponent& getServerPlayer();
	PhysicsComponent & getPhysics();
	PositionComponent & getPosition();
	PlayerStateComponent & getPlayerState();
	CombatComponent & getCombat();
	OnlineComponent& getOnline();

private:
	EntityId id;
	PeerId peerId;
	ConnectionPtr con;
};