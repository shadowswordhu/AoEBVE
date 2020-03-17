#pragma once
#include "ATOState.h"

#include "atsplugin.h"
#include "SpeedCurve.h"

#include <vector>
#include <algorithm>
#include <queue>
#include <fstream>
#include <filesystem>
#include <string>
#include <cstdio>

class ATO
{
	
private:
	
	// FILE* fout;
	double K_P = 1, K_I = 0.02, K_D = -0.01;
	const double eps_stop = 0.3;
	const double train_length = 150;
	double lastError = 0;
	int lastTime = 0;
	double integratedErr = 0, integrateRoof = 10, integrateFloor = -1000;

	ATS_HANDLES* output = NULL;
	ATS_VEHICLESPEC* vehicleSpec = NULL;
	ATS_VEHICLESTATE* vehicleState = NULL;
	
	int* powerNotch;
	int* brakeNotch;

	std::vector<SpeedCurve> speedCurves;
	int activeBegin = 0, activeEnd = 0;

	void loadProfile();

public:
	ATOState* state = NULL;

	ATO(ATS_HANDLES* _output, ATS_VEHICLESPEC* _vehicleSpec, ATS_VEHICLESTATE* _vehicleState,
		int* _powerNotch, int* _brakeNotch) {
		output = _output;
		vehicleState = _vehicleState;
		vehicleSpec = _vehicleSpec;
		powerNotch = _powerNotch;
		brakeNotch = _brakeNotch;
		loadProfile();
		setState(new ATOReadyState(this));
		// fout = fopen("R:\\Softwares\\BveTs\\Scenarios\\JRTozai\\207-1000\\test.out", "w");
	}
	void setState(ATOState* _state) {
		delete state;
		state = _state;
	}
	
	int getSpeedCurveAmount() {
		//return (int)speedCurves.size();
		return activeEnd;
	}

	int getBrakeOutput() {
		return output->Brake;
	}

	double getCurrentSpeed() {
		return (double)vehicleState->Speed;
	}

	void setATSHandle(double brake, double power) {
		output->Brake = (int)(round(brake * (double)(vehicleSpec->BrakeNotches) + .1));
		output->Power = (int)(round(power * (double)(vehicleSpec->PowerNotches) + .1));
	}

	double getBrakeHandlePosition() {
		return (double)* brakeNotch / (double)vehicleSpec->BrakeNotches;
	}
	double getPowerHandlePosition() {
		return (double)* powerNotch / (double)vehicleSpec->PowerNotches;
	}

	void setATSBrakeHandle(const int &brake) {
		output->Brake = brake;
	}

	void setATSPowerHandle(const int &power) {
		output->Power = power;
	}

	bool isTrainStopped() {
		return vehicleState->Speed < 1e-5;
	}

	double getSpeedTarget();
	void finishStop();
	double followSpeed(double speed);
	double dbg_ctrlOutput;
};
