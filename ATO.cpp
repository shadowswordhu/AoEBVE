#include "stdafx.h"
#include "ATO.h"

EXTERN_C IMAGE_DOS_HEADER __ImageBase;

void ATO::loadProfile() { // TODO: Load from file
	speedCurves.clear();
	
	std::wofstream inputFiles("R:\\Softwares\\BveTs\\Scenarios\\JRTozai\\207-1000\\ATOProfile.out");
	WCHAR DllPathStr[MAX_PATH] = { 0 };
	GetModuleFileNameW((HINSTANCE)& __ImageBase, DllPathStr, _countof(DllPathStr));
	std::experimental::filesystem::path dllPath(DllPathStr);
	std::experimental::filesystem::path nameOfProfileName("ATOProfile.in");
	std::wifstream profileNameInput(dllPath.parent_path().append(nameOfProfileName).string());
	std::wstring fileName;
	profileNameInput >> fileName;
	profileNameInput.close();

	std::ifstream profileInput(fileName);
	
	double param1, param2;
	std::string param3;
	while (!profileInput.eof()) {
		profileInput >> param1 >> param2 >> param3;
		if (param1 > 10) param1 -= 2.0;
		if (param3[0] == 's') {
			SpeedCurve curve(param1, param2);
			speedCurves.push_back(curve);
		}
		else {
			double p3;
			p3 = stof(param3);
			SpeedCurve curve(param1, param2, p3 + train_length);
			speedCurves.push_back(curve);
		}
	}
	
	profileInput.close();
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