#pragma once
#include "Command.h"
#include "WeaponManager.h"
#include "Client.h"
#include "Game.h"

class WeaponCommand : public Command {
public:
	WeaponCommand(Game& game);
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	WeaponManager* weapons;
	Client* client;
};