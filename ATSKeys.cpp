#include "stdafx.h"
#include "ATSKeys.h"

void ATSKeys::update(int atsKey, bool Status, int timeStamp) {
	this->status[atsKey] = Status;
	this->timeStamp[atsKey] = timeStamp;
}

bool ATSKeys::getStatus(int atsKey) {
	return this->status[atsKey];
}

int ATSKeys::getTimeStamp(int atsKey) {
	return this->status ? this->timeStamp[atsKey] : -1;
}