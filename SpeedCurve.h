#pragma once
#include <cstdio>
class SpeedCurve
{
private:
	double maxSpeedKmh = 130;

	//FILE* fout;

	double maxDeceleration = 0.75;          // m/(s^2)
	double decelerationRate = 0.08;          // m/(s^3)
	double defaultExpireDistance = 1000000; // m
	double maxRange = 0;

	double dynamicReactionTime = 10;        // s
	double targetSpeed;                     // m/s
	double targetPosition;                  // m
	double expirePosition;                  // m

	double startSpeed = maxSpeedKmh / 3.6;
	
	double t_tr = 0, v_tr = 0, t_const_a = 0, t_total = 0;
	
	void calc_param();

	double v_t(double t) const;
public:
	double x_t(double t) const;

public:
	SpeedCurve(double _targetSpeed, double _targetPosition, double _expirePosition = -1) {
		targetSpeed = _targetSpeed / 3.6;
		targetPosition = _targetPosition;
		if (_expirePosition < 0) {
			expirePosition = targetPosition + defaultExpireDistance;
		}
		else {
			expirePosition = _expirePosition;
		}
		
		calc_param();
		maxRange = targetPosition - (x_t(t_total) + dynamicReactionTime * maxSpeedKmh / 3.6);
	//DEBUG: ONLY FOR ONE SPEEDCURVE
		//fout = fopen("R:\\Softwares\\BveTs\\Scenarios\\mackoy.Keisei\\K_SEI3500R\\SCtest.txt", "w");
	}

	double getRange(double speed, bool kmh);
	double getMaxRange() const;
	void setStartSpeed(double speed, bool kmh);
	double getSpeed(double position);
	double getTimeOfArrival(double position);
	double getDeceleration(double position);
	bool isExpired(double position);
	void setExpired();


	friend bool operator< (const SpeedCurve& a, const SpeedCurve& b) {
		return a.getMaxRange() < b.getMaxRange();
	}
};

