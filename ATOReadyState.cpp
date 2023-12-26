#include "stdafx.h"
#include "ATOState.h"
#include "ATO.h"

void ATOReadyState::start() {
	//context->finishStop();
	context->getSpeedTarget(0);
	context->setState(new ATOEngagedState(context));
}

void ATOReadyState::run() {
	context->setATOLampBlink();
	if (context->isTrainStopped()) {
		context->setATSHandle(max(context->getBrakeHandlePosition(), 0.6), 0);
	}
}

void ATOReadyState::setBrake(int brake) {
	
}

void ATOReadyState::setPower(int power) {
	if (context->getBrakeHandlePosition() < 1e-5 && power > 0) {
		context->setATSPowerHandle(power);
		context->setState(new ATOAvailableState(context));
	}
}
