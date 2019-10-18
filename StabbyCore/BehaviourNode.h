#pragma once
#include "BehaviourTree.h"
#include <vector>

enum class Result {
	running,
	success,
	failure
};

class BehaviourNode {
	friend class BehaviourTree;

public:
	
private:
	BehaviourNode* parent;
	std::vector<BehaviourNode*> children;
};