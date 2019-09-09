#include "stdafx.h"
#include "AttackManager.h"

#include <filesystem>
#include <fstream>

namespace fs = std::experimental::filesystem;

void AttackManager::loadAttacks(const std::string & attackDir) {
	fs::path path{ attackDir };
	for (auto & file : fs::directory_iterator(path)) {
		if (file.path().extension() == ".json") {
			std::ifstream stream{ file.path().string() };
			json jsonFile;
			stream >> jsonFile;
			
			int restartDelayMax = jsonFile["restartDelay"];

			Attack attack{restartDelayMax};

			for (auto & hitbox : jsonFile["hitboxes"]) {
				Vec2f res{}, offset{};
				
				res.x = hitbox["width"];
				res.y = hitbox["height"];
				offset.x = hitbox["offset_x"];
				offset.y = hitbox["offset_y"];

				unsigned int startup = hitbox["startup"];
				unsigned int active = hitbox["active"];
				unsigned int ending = hitbox["ending"];

				attack.addHitbox(Hitbox{ AABB{{0 ,0}, res}, offset, startup, active, ending });
			}

			attacks.insert(std::pair{ file.path().stem().string(), attack });
		}
	}
}

Attack AttackManager::cloneAttack(const std::string & attackId) {
	return attacks[attackId];
}
