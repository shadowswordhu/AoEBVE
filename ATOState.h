#pragma once
class ATO;

class ATOState
{
public:
	virtual void start() = 0;
	virtual void revokeStart() = 0;
	virtual void disengage() = 0;
	virtual void run() = 0;
};


class ATOServiceBrakeState :
	public ATOState
{
private:
	ATO* context;
public:
	ATOServiceBrakeState(ATO* ato) {
		context = ato;
	}
	void start() override {}
	void revokeStart() override {}
	void disengage() override {}
	void run() override;
};

class ATOReadyState :
	public ATOState
{
private:
	ATO* context;
public:
	ATOReadyState(ATO* ato) {
		context = ato;
	}
	void start() override;
	void revokeStart() override {}
	void disengage() override {}
	void run() override;
};

class ATOEngagedState :
	public ATOState
{
private:
	ATO* context;
public:
	ATOEngagedState(ATO* ato) {
		context = ato;
	}
	void start() override {}
	void revokeStart() override;
	void disengage() override;
	void run() override;
};
