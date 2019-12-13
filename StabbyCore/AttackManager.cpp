#include "stdafx.h"
#include "AttackManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>

namespace fs = std::filesystem;

void AttackManager::loadAttacks(const std::string & attackDir) {
	fs::path path{ attackDir };
	for (auto & file : fs::directory_iterator(path)) {
		if (file.path().extension() == ".json") {
			std::ifstream stream{ file.path().string() };
			if (stream.is_open()) {
				try {
					json jsonFile;
					stream >> jsonFile;

					int restartDelayMax = jsonFile["restartDelay"];

					Attack attack{file.path().stem().string(), restartDelayMax };

					for (auto& hitbox : jsonFile["hitboxes"]) {
						Vec2f res{}, offset{};

						res.x = hitbox["width"];
						res.y = hitbox["height"];
						offset.x = hitbox["offset_x"];
						offset.y = hitbox["offset_y"];

						unsigned int startup = hitbox["startup"];
						unsigned int active = hitbox["active"];
						unsigned int ending = hitbox["ending"];
						unsigned int stun = hitbox["stun"];
						float damage = hitbox["damage"];

						attack.addHitbox(Hitbox{ AABB{{0 ,0}, res}, offset, startup, active, ending, stun, damage });
					}

					attacks.insert(std::pair{ file.path().stem().string(), attack });
				}
				catch (std::exception e) {
					std::cout << "An error occurred while reading the file \"" << file.path() << "\"\n";
					std::cout << e.what() << '\n';
				}
			}
			else {
				std::cout << "Unable to open file \"" << file.path() << "\"\n";
			}
		}
	}
}

Attack AttackManager::cloneAttack(const std::string & attackId) {
	return attacks[attackId];
}

attack_iterator AttackManager::begin() {
	return attacks.begin();
}

attack_iterator AttackManager::end() {
	return attacks.end();
}

bool AttackManager::hasAttack(const std::string& attackId) {
	return attacks.find(attackId) != attacks.end();
}
