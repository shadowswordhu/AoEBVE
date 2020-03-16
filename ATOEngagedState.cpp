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
	if (context->getCurrentSpeed() < 1e-5) {
		context->setState(new ATOReadyState(context));
	}
}
