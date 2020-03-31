#pragma once
#include "Command.h"
#include "PaletteManager.h"

class PaletteCommand : public Command {
public:
	// Inherited via Command
	PaletteCommand(PaletteManager* palettes);
	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;
private:
	PaletteManager* palettes;
};