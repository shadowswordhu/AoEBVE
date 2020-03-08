#include "stdafx.h"
#include "ATO.h"

#include <cstring>

void ATO::loadProfile() { // TODO: Load from file
	speedCurves.clear();
	
	FILE* fin = fopen("R:\\Softwares\\BveTs\\Scenarios\\JRTozai\\207-1000\\ATOProfile.in", "r");
	char fileName[256];
	fscanf(fin, "%s", fileName);
	fclose(fin);
	
	fin = fopen(fileName, "r");
	
	double param1, param2;
	char param3[128];
	while (fscanf(fin, "%lf%lf%s", &param1, &param2, param3) != EOF) {
		if (param1 > 10) param1 -= 2.0;
		if (param3[0] == 's') {
			SpeedCurve curve(param1, param2);
			speedCurves.push_back(curve);
		}
		else {
			double p3;
			sscanf(param3, "%lf", &p3);
			SpeedCurve curve(param1, param2, p3 + train_length);
			speedCurves.push_back(curve);
		}
	}
	
	fclose(fin);
	std::sort(speedCurves.begin(), speedCurves.end());
}

double ATO::getSpeedTarget() {
	double result = 1000;

	for (; activeBegin < activeEnd; ++activeBegin) {
		if (!speedCurves[activeBegin].isExpired(vehicleState->Location)) {
			break;
		}
	}

	for (int i = activeBegin; i < activeEnd; ++i) {
		if (speedCurves[i].isExpired(vehicleState->Location)) {
			continue;
		}
		else {
			double spd = speedCurves[i].getSpeed(vehicleState->Location);
			result = spd < result ? spd : result;
		}
	}

	for (int i = activeEnd; i < (int)speedCurves.size(); ++i) {
		if (speedCurves[i].getMaxRange() > vehicleState->Location) {
			break;
		}

		if (speedCurves[i].getRange(vehicleState->Speed, 1) < vehicleState->Location) {

			speedCurves[i].setStartSpeed(vehicleState->Speed, 1);
			activeEnd = i + 1;
		}
	}
	return result;
}

void ATO::finishStop() {
	for (int i = activeBegin; i < activeEnd; ++i) {
		if (!speedCurves[i].isExpired(vehicleState->Location)
			&& speedCurves[i].getSpeed(vehicleState->Location) < 0.5) {
			speedCurves[i].setExpired();
			break;
		}
	}
}

double ATO::followSpeed(double speed) { // PID
	double curErr = speed - vehicleState->Speed / 3.6;
	int curTime = vehicleState->Time;
	double deltaT = ((double)curTime - (double)lastTime) / 1000.0;
	integratedErr += deltaT * (curErr + lastError) / 2;
	integratedErr = min(integratedErr, integrateRoof);
	integratedErr = max(integratedErr, integrateFloor);
	double controlOutput = K_P * curErr + K_I * integratedErr + K_D * (curErr - lastError) / deltaT;

	if (controlOutput > 0) {
		setATSHandle(0, controlOutput < 1 ? controlOutput : 1);
	}
	else {
		setATSHandle((-controlOutput) < 1 ? (-controlOutput) : 1, 0);
	}

	lastError = curErr;
	lastTime = curTime;
	dbg_ctrlOutput = speed;
	return controlOutput;
}