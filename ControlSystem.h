#pragma once
#include "Supervision.h"
#include "Command.h"
#include "Enforcement.h"

#include <list>

class ControlSystem
{
protected:
	bool power;
	std::list<Supervision*> supervisions;
	std::list<Enforcement*> enforcements;

public:
	virtual void setCommand(Command* ) = 0;
	virtual void turnOn() = 0;
	virtual void turnOff() = 0;
	virtual void run() = 0;
};
