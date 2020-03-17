#include "stdafx.h"
#include "ATOState.h"
#include "ATO.h"

void ATOAvailableState::run() {

	if (context->isTrainStopped() && context->getPowerHandlePosition() < 1e-5) {
		context->setState(new ATOReadyState(context));
	}
}

void ATOAvailableState::setBrake(int brake) {
	context->setATSBrakeHandle(brake);
}

void ATOAvailableState::setPower(int power) {
	context->setATSPowerHandle(power);
}
