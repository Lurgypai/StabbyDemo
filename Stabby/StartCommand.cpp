#include "pch.h"
#include "StartCommand.h"
#include "DebugIO.h"

StartCommand::StartCommand(Game & game_) :
	game{game_}
{}

std::string StartCommand::getTag() const {
	return "startgame";
}

void StartCommand::onCommand(const std::vector<std::string>& args) {
	if (args.size() == 2) {
		if (args[1] == "true") {
			DebugIO::printLine("Incorrect amount of args.");
		}
		else if (args[1] == "false") {
			game.startOfflineGame();
		}
	}
	else if (args.size() == 4) {
		if (args[1] == "true") {
			try {
				int port = std::stoi(args[3]);
				game.startOnlineGame(args[2], port);
			}
			catch (std::invalid_argument e) {
				DebugIO::printLine("Invalid argument.");
			}
		}
		else if (args[1] == "false") {
			DebugIO::printLine("Incorrect amount of args.");
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
