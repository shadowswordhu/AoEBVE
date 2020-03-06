#pragma once
#include "ControlSystem.h"
class Command
{
protected:
	bool pending;
	ControlSystem *controlSystem;

public:
	virtual void execute() = 0;

};
