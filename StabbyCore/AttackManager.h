#pragma once
#include <string>
#include <unordered_map>

#include "Attack.h"

class AttackManager {
public:
	void loadAttacks(const std::string & attackDir);
	Attack cloneAttack(const std::string & attackId);
private:
	std::unordered_map<std::string, Attack> attacks;
};