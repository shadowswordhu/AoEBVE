#include "stdafx.h"
#include "ATOState.h"
#include "ATO.h"

void ATOEngagedState::revokeStart() {
	context->setState(new ATOServiceBrakeState(context));
}

void ATOEngagedState::disengage() {
	context->setState(new ATOReadyState(context));
}

void ATOEngagedState::run() {
	double speedTarget = context->getSpeedTarget(0);
	double speedTarget_delay = context->getSpeedTarget(context->t_traction_reset);
	context->followSpeed(speedTarget, speedTarget_delay);
	context->setATOLampOn();

	if (context->isTrainStopped()) {
		context->setState(new ATOReadyState(context));
	}
}

void ATOEngagedState::setBrake(int brake) {
	if (brake > 0) {
		context->setState(new ATOAvailableState(context));
	}
}

void ATOEngagedState::setPower(int power) {

}
