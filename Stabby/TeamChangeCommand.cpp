#include "pch.h"
#include "TeamChangeCommand.h"
#include <TeamChangePacket.h>
#include <DebugIO.h>

TeamChangeCommand::TeamChangeCommand(Client* client_) :
	client{ client_ }
{}

std::string TeamChangeCommand::getTag() const {
	return "team";
}

void TeamChangeCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		uint64_t newTeam = std::stoi(args[1]);
		TeamChangePacket p;
		p.id = client->getNetId();
		p.targetTeamId = newTeam;

		client->send(p);
		DebugIO::printLine("Requested team change to: " + std::to_string(newTeam));
	}
}
