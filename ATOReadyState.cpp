#include "stdafx.h"
#include "ATOReadyState.h"

void ATOReadyState::start() {
	context->finishStop();
	context->setState(new ATOEngagedState(context));
}

void ATOReadyState::run() {

	if (context->getCurrentSpeed() < 1e-5) {
		context->setATSHandle(max(context->getBrakeHandlePosition(), 0.6), 0);
	}
}
