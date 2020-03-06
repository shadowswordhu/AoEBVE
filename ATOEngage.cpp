#include "stdafx.h"
#include "ATOEngage.h"

void ATOEngage::checkInput(int Time) {
	
	if (pressedTime == -1) {
		if (atsKeys->getStatus(keyA) && atsKeys->getStatus(keyB)) {
			pressedTime = Time;
		}
	}

	else {
		if (atsKeys->getStatus(keyA) && atsKeys->getStatus(keyB)) {
			if (Time - pressedTime > timeStartATO) {
				atoInvoke();
			}
		}
		else {
			if (Time - pressedTime <= timeRevokeATO) {
				atoRevoke();
			}
			pressedTime = -1;
		}
	}
}

inline void ATOEngage::atoInvoke() {
	ato->state->start();
}

inline void ATOEngage::atoRevoke() {
	ato->state->revokeStart();
}
