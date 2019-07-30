#pragma once
#include "ZombieLC.h"
#include <vector>
#include "NetworkTypes.h"

class ClientZombieLC : public ZombieLC	{
public:
	ClientZombieLC(EntityId id_ = 0);
	void runLogic(Time_t now);
private:
	unsigned int start;
	unsigned int end;
	std::vector<ZombieState> prevStates;
	Time_t last;
};