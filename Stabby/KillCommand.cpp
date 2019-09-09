#include "pch.h"
#include "KillCommand.h"
#include "ClientPlayerLC.h"
#include "EntityBaseComponent.h"

std::string KillCommand::getTag() const {
	return "kill";
}

void KillCommand::onCommand(const std::vector<std::string>& args) {
	for (auto& player : EntitySystem::GetPool<ClientPlayerLC>()) {
		EntitySystem::GetComp<EntityBaseComponent>(player.getId())->isDead = true;
	}
}
