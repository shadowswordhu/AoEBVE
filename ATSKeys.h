#pragma once
class ATSKeys
{
private:
	bool status[16] = {0}; // Pressed / DOWN = 1, Released / UP = 0
	int timeStamp[16] = {0};

public:

	ATSKeys() {
		for (int i = 0; i < 16; ++i) {
			status[i] = 0;
			timeStamp[i] = 0;
		}
	}

	// TODO: Check destroy functions
	void update(int atsKey, bool status, int timeStamp);
	bool getStatus(int atsKey);
	int getTimeStamp(int atsKey);
};

