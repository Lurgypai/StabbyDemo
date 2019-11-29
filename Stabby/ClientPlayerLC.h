#pragma once
#include "PlayerLC.h"
#include "NetworkTypes.h"
#include <deque>

#define BUFFER_SIZE 200

//input and value state at a specific time
struct TotalPlayerState {
	PlayerState plr;
	unsigned char in;
};

class ClientPlayerLC : public PlayerLC {
public:
	ClientPlayerLC(EntityId id_ = 0);
	void update(Time_t clientTime, Time_t gameTime, double timeDelta, const Controller & controller);
	//Repredict current loc using correct position at time when.
	bool repredict(const PlayerState & state);
	void setPhysics(PhysicsSystem & physics);
	void setCombat(CombatSystem & combat);
	std::string getHeadPath();
	Vec2f getCenter();
private:
	//client sender tick of last acknowledged input
	Time_t last;
	std::deque<TotalPlayerState> states;
	PhysicsSystem * physicsSystem;
	CombatSystem * combatSystem;
};
