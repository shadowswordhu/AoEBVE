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
	double speedTarget = context->getSpeedTarget();
	context->followSpeed(speedTarget);
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
