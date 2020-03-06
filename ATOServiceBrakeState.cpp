#include "stdafx.h"
#include "ATOServiceBrakeState.h"

void ATOServiceBrakeState::run() {
	context->setATSHandle(1.0, 0);
	if (context->getCurrentSpeed() < 1e-5) {
		context->setState(new ATOReadyState(context));
	}
}
