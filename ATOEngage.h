#pragma once
#include "Command.h"

#include "ATO.h"
#include "ATSKeys.h"


class ATOEngage :
	public Command
{
private:
	ATO* ato = NULL;
	ATSKeys* atsKeys = NULL;

	void atoInvoke();
	void atoRevoke();

	int pressedTime = -1;

	const int timeStartATO = 1000; // ms
	const int timeRevokeATO = 3000; // ms
	const int keyA = 1; // TODO: use ATS Keys definition
	const int keyB = 3;


public:

	ATOEngage(ATO* Ato, ATSKeys *AtsKeys) {
		ato = Ato;
		atsKeys = AtsKeys;
	}

	ATOEngage() = delete;
	
	void checkInput(int Time);
};
