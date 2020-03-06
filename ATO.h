#pragma once
#include "ATOState.h"




#include "atsplugin.h"
#include "SpeedCurve.h"

#include <vector>
#include <algorithm>
#include <queue>

#include <cstdio>

class ATO
{
	
private:
	
	FILE* fout;

	//ATODisengagedState disengaged;
	//ATOServiceBrakeState serviceBrake;

	double K_P = 0.8, K_I = 0.01, K_D = -0.03;
	const double eps_stop = 0.3;
	double lastError = 0;
	int lastTime = 0;
	double integratedErr = 0, integrateRoof = 10;

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
		//setState(new ATOReadyState(this));
		fout = fopen("R:\\Softwares\\BveTs\\Scenarios\\mackoy.Keisei\\K_SEI3500R\\test.txt", "w");
	}
	void setState(ATOState* _state) {
		delete state;
		state = _state;
	}
	
	double getCurrentSpeed() {
		return (double)vehicleState->Speed;
	}

	void setATSHandle(double brake, double power) {
		output->Brake = (int)(round(brake * vehicleSpec->BrakeNotches + .1));
		output->Power = (int)(round(power * vehicleSpec->PowerNotches + .1));
	}

	double getBrakeHandlePosition() {
		return (double)* brakeNotch / (double)vehicleSpec->BrakeNotches;
	}
	double getPowerHandlePosition() {
		return (double)* powerNotch / (double)vehicleSpec->PowerNotches;
	}

	double getSpeedTarget() {
		double result = 1000;

		for (; activeBegin < activeEnd; ++activeBegin) {
			if (!speedCurves[activeBegin].isExpired(vehicleState->Location)) {
				break;
			}
		}

		for (int i = activeBegin; i < activeEnd; ++i) {
			if (speedCurves[i].isExpired(vehicleState->Location)) {
				//fprintf(fout, "Expired?\n");
				continue;
			}
			else {
				double spd = speedCurves[i].getSpeed(vehicleState->Location);
				//fprintf(fout, "%d,%f, %f,%f,%f\n", vehicleState->Time, 
				//	vehicleState->Location, 
				//	speedCurves[i].getTimeOfArrival(vehicleState->Location),
				//	spd,
				//	speedCurves[i].x_t(speedCurves[i].getTimeOfArrival(vehicleState->Location)));
				
				
				result = spd < result ? spd : result;
			}
		}

		for (int i = activeEnd; i < (int)speedCurves.size(); ++i) {
			if (speedCurves[i].getMaxRange() > vehicleState->Location) {
				break;
			}
			//fprintf(fout, "%d,%f,%f\n", vehicleState->Time, vehicleState->Location,vehicleState->Speed);
			if (speedCurves[i].getRange(vehicleState->Speed, 1) < vehicleState->Location) {
				
				speedCurves[i].setStartSpeed(vehicleState->Speed, 1);
				activeEnd = i + 1;
			}
		}
		return result;
	}

	void finishStop() {
		for (int i = activeBegin; i < activeEnd; ++i) {
			if (!speedCurves[i].isExpired(vehicleState->Location) 
				&& speedCurves[i].getSpeed(vehicleState->Location) < 0.5) {
				speedCurves[i].setExpired();
				break;
			}
		}
	}

	double followSpeed(double speed) { // PID
		double curErr = speed - vehicleState->Speed / 3.6;
		int curTime = vehicleState->Time;
		double deltaT = ((double)curTime - (double)lastTime)/1000.0;
		integratedErr += deltaT * (curErr + lastError) / 2;
		integratedErr = min(integratedErr, integrateRoof);

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
	double dbg_ctrlOutput;
};

