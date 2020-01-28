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
	if (args.size() >= 2) {
		if (args[1] == "0") {
			if (args.size() == 3) {
				game.startOfflineGame(args[2]);
			}
			else {
				DebugIO::printLine("Incorrect amount of args.");
			}
		}
		else if (args[1] == "1") {
			if (args.size() == 5) {
				try {
					int port = std::stoi(args[3]);
					game.startOnlineGame(args[2], port, args[4]);
				}
				catch (std::invalid_argument e) {
					DebugIO::printLine("Invalid argument.");
				}
			}
			else {
				DebugIO::printLine("Incorrect amount of args.");
			}
		}
		else if (args[1] == "2") {
			if (args.size() == 3) {
				game.startStageEditor(args[2]);
			}
			else {
				DebugIO::printLine("Incorrect amount of args.");
			}
		}
	}
	else {
		DebugIO::printLine("Incorrect amount of args.");
	}
}
