#include "stdafx.h"
#include "ATOState.h"
#include "ATO.h"

void ATOServiceBrakeState::run() {
	context->setATOLampOff();
	context->setATSHandle(1.0, 0);
	if (context->isTrainStopped()) {
		context->setState(new ATOReadyState(context));
	}
}
