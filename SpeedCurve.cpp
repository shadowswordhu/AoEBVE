#include "stdafx.h"
#include "SpeedCurve.h"

#include <cmath>

void SpeedCurve::calc_param() {
	t_tr = maxDeceleration/decelerationRate;
	
	double diff_Speed = startSpeed - targetSpeed;
	if (diff_Speed < 0) {
		t_tr = 0;
		t_const_a = 0;
		v_tr = 0;
	}
	else if (diff_Speed < decelerationRate * t_tr * t_tr) {
		t_tr = sqrt(diff_Speed / decelerationRate);
		t_const_a = 0;
		v_tr = diff_Speed / 2;
	} 
	else {
		v_tr = decelerationRate * t_tr * t_tr / 2;
		t_const_a = (diff_Speed - 2 * v_tr) / maxDeceleration;
	}
	t_total = t_tr * 2 + t_const_a;
}

double SpeedCurve::v_t(double t) const{
	double v;
	if (t < t_tr) {
		v = decelerationRate * t * t / 2 + targetSpeed;
	}
	else if (t < t_tr + t_const_a) {
		v = v_tr + (t - t_tr) * maxDeceleration + targetSpeed;
	}
	else if (t < t_tr + t_tr + t_const_a) {
		double rest_t = t_const_a + t_tr + t_tr - t;
		v = startSpeed - decelerationRate * rest_t * rest_t / 2;
	}
	else {
		v = startSpeed;
	}
	return v;
}

double SpeedCurve::x_t(double t) const{
	
	double x;
	if (t < t_tr) {
		x = decelerationRate * t * t * t / 6;
	}
	else if (t < t_tr + t_const_a) {
		x = decelerationRate * t_tr * t_tr * t_tr / 6
			+ maxDeceleration * (t - t_tr) * (t - t_tr) / 2 + v_tr * (t - t_tr);
	}
	else if (t < t_tr + t_tr + t_const_a) {
		double rest_t = t_total - t;
		x = decelerationRate * t_tr * t_tr * t_tr / 6
			+ maxDeceleration * t_const_a * t_const_a / 2 + v_tr * t_const_a
			+ (startSpeed - targetSpeed) * (t - t_tr - t_const_a)
			+ decelerationRate * rest_t * rest_t * rest_t / 6
			- decelerationRate * t_tr * t_tr * t_tr / 6;
	}
	else {
		x = decelerationRate * t_tr * t_tr * t_tr / 6
			+ maxDeceleration * t_const_a * t_const_a / 2 + v_tr * t_const_a
			+ (startSpeed - targetSpeed) * t_tr
			- decelerationRate * t_tr * t_tr * t_tr / 6
			+ (startSpeed - targetSpeed) * (t - t_total);
	}
	return x + targetSpeed * t;
}

double SpeedCurve::getRange(double speed, bool kmh) {
	if (kmh) {
		speed = speed / 3.6;
	}
	
	startSpeed = speed;
	calc_param();

	if (speed < targetSpeed) {
		return targetPosition;
	}

	return targetPosition - (x_t(t_total) + dynamicReactionTime * speed);
}
double SpeedCurve::getMaxRange() const {

	return maxRange;
}

void SpeedCurve::setStartSpeed(double speed, bool kmh) {
	if (kmh) {
		speed = speed / 3.6;
	}
	startSpeed = speed;
	calc_param();
}

double SpeedCurve::getSpeed(double position) {
	
	if (position < targetPosition) {
		return v_t(getTimeOfArrival(position));
	}
	else {
		return targetSpeed;
	}
}
	

double SpeedCurve::getDeceleration(double position) {
	return 0; // TODO
}

double SpeedCurve::getTimeOfArrival(double position) {
	if (position > targetPosition) {
		return 0;
	}
	
	double tl = 0, tr = t_total, tmid = (tl+tr)/2;
	double xl = 0, xr = x_t(t_total);
	double d = targetPosition - position;
	double d_xtra = d - xr;
	if (d_xtra > 0) {
		return t_total + d_xtra / startSpeed;
	}

	for (int it = 0; it < 14; ++it) {
		tmid = (tl + tr) / 2;
		double xmid = x_t(tmid);
		if (xmid < d) {
			xl = xmid;
			tl = tmid;
		}
		else {
			xr = xmid;
			tr = tmid;
		}
	}

	//fprintf(fout, "%f, %f, %f, %f, %f\n", d, xl, tl, xr, tr);

	return tl;
}

bool SpeedCurve::isExpired(double position) {
	if (position > expirePosition) {
		return true;
	}
	return false;
}

void SpeedCurve::setExpired() {
	expirePosition = -100;
}
