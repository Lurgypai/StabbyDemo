#include "pch.h"
#include "KillCommand.h"
#include "PlayerLC.h"
#include "EntityBaseComponent.h"

std::string KillCommand::getTag() const {
	return "kill";
}

void KillCommand::onCommand(const std::vector<std::string>& args) {
	for (auto& player : EntitySystem::GetPool<PlayerLC>()) {
		EntitySystem::GetComp<EntityBaseComponent>(player.getId())->isDead = true;
	}
}
