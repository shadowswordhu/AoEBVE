#include "stdafx.h"
#include "ATOEngagedState.h"

void ATOEngagedState::revokeStart() {
	context->setState(new ATOServiceBrakeState(context));
}

void ATOEngagedState::disengage() {
	context->setState(new ATOReadyState(context));
}

void ATOEngagedState::run() {
	double speedTarget = context->getSpeedTarget();
	context->followSpeed(speedTarget);
	if (context->getCurrentSpeed() < 1e-5 && speedTarget < 5/3.6) {
		context->setState(new ATOReadyState(context));
	}
}
