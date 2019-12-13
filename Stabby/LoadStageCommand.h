#pragma once
#include "Command.h"
#include <EditableSystem.h>

class LoadStageCommand : public Command {
public:
	LoadStageCommand(EditableSystem& editables_);

	virtual std::string getTag() const override;
	virtual void onCommand(const std::vector<std::string>& args) override;

private:
	EditableSystem& editables;
};