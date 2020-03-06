#include "stdafx.h"
#include "ATO.h"



void ATO::loadProfile() { // TODO: Load from file
	speedCurves.clear();

	SpeedCurve stop0(0, 23),
		stop1(0,980),
		stop2(0, 1805),
		stop3(0, 2345);
	SpeedCurve limit0(90, 0),
		limit1(50, 2010, 2165),
		limit2(55, 2175);
	speedCurves.push_back(stop1);
	
	speedCurves.push_back(stop1);
	speedCurves.push_back(stop2);
	speedCurves.push_back(stop3);
	speedCurves.push_back(limit0);
	speedCurves.push_back(limit1);
	speedCurves.push_back(limit2);


	std::sort(speedCurves.begin(), speedCurves.end());
}
