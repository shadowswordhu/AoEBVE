// Ats.cpp : DLL �A�v���P�[�V�����p�̃G���g�� �|�C���g���`���܂��B
//
#include "stdafx.h"
#include "Ats.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    switch (ul_reason_for_call)
	{
		case DLL_PROCESS_ATTACH:
			g_atssn.Time = &g_time;
			g_atssn.Notch = &g_brakeNotch;

			g_atsp.TrainSpeed = &g_speed;
			g_atsp.DeltaT = &g_deltaT;
			g_atsp.BrakeNotch = &g_brakeNotch;
			g_atsp.Reverser = &g_reverser;

			g_spp.TrainSpeed = &g_speed;
			g_spp.DeltaT = &g_deltaT;
			g_spp.BrakeNotch = &g_brakeNotch;

			g_ato = new ATO(&g_output, &g_vehicleSpec, &g_vehicleState, &g_powerNotch, &g_brakeNotch);
			g_ato->setState(new ATOReadyState(g_ato));
			g_atoEngage = new ATOEngage(g_ato, &g_atskeys);
			
			//dmi = new DMIConnection;
			//OutputDebugStringA("DMI created!\n");
			//dmi->Connect();
			//OutputDebugStringA("DMI connected!\n");
			break;
		case DLL_THREAD_ATTACH:
		case DLL_THREAD_DETACH:
		case DLL_PROCESS_DETACH:

			// delete dmi;
			break;
    }
    return TRUE;
}

ATS_API int WINAPI GetPluginVersion()
{
	return ATS_VERSION;
}

ATS_API void WINAPI SetVehicleSpec(ATS_VEHICLESPEC vehicleSpec)
{
	g_vehicleSpec = vehicleSpec;
	g_svcBrake = vehicleSpec.BrakeNotches;
	g_emgBrake = g_svcBrake + 1;
	g_atssn.EmgNotch = g_emgBrake;
	g_atssn.CancelNotch = vehicleSpec.AtsNotch;
	g_atsp.EmergencyNotch = g_emgBrake;
	g_atsp.ServiceNotch = vehicleSpec.AtsNotch;
	g_spp.ServiceNotch = vehicleSpec.AtsNotch;
}

ATS_API void WINAPI Initialize(int brake)
{
	
	g_atssn.InitSn();
	g_atsp.InitP();
	g_spp.InitSpp();
	g_speed = 0;
}

ATS_API ATS_HANDLES WINAPI Elapse(ATS_VEHICLESTATE vehicleState, int *panel, int *sound)
{
	g_deltaT = vehicleState.Time - g_time;
	g_time = vehicleState.Time;
	g_speed = vehicleState.Speed;
	g_vehicleState = vehicleState;

	//dmi->updateSpeed(vehicleState.Speed);


	// �n���h���o��
	if(g_atssn.AtsBrake || g_atsp.EmergencyBrake)
	{
		g_output.Brake = g_emgBrake;
	}
	else if(g_atsp.ServiceBrake && g_brakeNotch != g_emgBrake)
	{
		g_output.Brake = g_svcBrake;
	}
	else
	{
		g_output.Brake = g_brakeNotch;
	}
	if(g_pilotlamp)
	{
		g_output.Reverser = g_reverser;
	}
	else
	{
		g_output.Reverser = 0;
	}
	g_output.Power = g_powerNotch;
	g_output.ConstantSpeed = ATS_CONSTANTSPEED_CONTINUE;

	g_atoEngage->checkInput(g_time);
	g_ato->state->run();

	g_atssn.RunSn(); // ATS-SN
	g_atsp.RunP(); // ATS-P
	g_spp.RunSpp(); // ��ԉw�ʉߖh�~���u

	// �p�l���o��
	panel[0] = g_atssn.WhiteLamp;
	panel[1] = g_atssn.RedLamp;
	panel[2] = 1; // P�d��
	panel[3] = g_atsp.ApproachLamp;
	panel[4] = 0; // �u���[�L�J��
	panel[5] = g_atsp.BrakeApplyLamp;
	panel[6] = g_atsp.AtspLamp;
	panel[7] = 0; // �̏�


	panel[51] = g_ato->getATOLamp();
	panel[201] = g_ato->getBrakeOutput();

	panel[233] = (int)(g_ato->getSpeedTarget(0) * 3.6);
	panel[230] = g_ato->getSpeedCurveAmount() % 10;
	panel[231] = g_ato->getSpeedCurveAmount() / 10;
	// �T�E���h�o��
	sound[0] = g_atssn.Bell;
	sound[1] = g_atssn.Chime;
	sound[2] = g_atsp.Bell;
	sound[6] = g_spp.HaltChime;
	sound[7] = g_spp.PassAlarm;

    return g_output;
}

ATS_API void WINAPI SetPower(int notch)
{
	g_powerNotch = notch;
	g_ato->state->setPower(notch);
}

ATS_API void WINAPI SetBrake(int notch)
{
	g_brakeNotch = notch;
	g_atssn.CheckAts();
	g_spp.NotchChanged();
	g_ato->state->setBrake(notch);
}

ATS_API void WINAPI SetReverser(int pos)
{
	g_reverser = pos;
}

ATS_API void WINAPI KeyDown(int atsKeyCode)
{
	g_atskeys.update(atsKeyCode, true, g_time);

	switch(atsKeyCode)
	{
	case ATS_KEY_S: // ATS �m�F
		g_atssn.DownButtom();
		break;
	case ATS_KEY_A1: // �x�񎝑�
		g_atssn.CancelChime();
		break;
	case ATS_KEY_B1: // ���A
		g_atssn.Reset();
		g_atsp.Reset();
		break;
	}
}

ATS_API void WINAPI KeyUp(int atsKeyCode)
{
	g_atskeys.update(atsKeyCode, false, g_time);

	if(atsKeyCode == ATS_KEY_S){g_atssn.UpButton();}
}

ATS_API void WINAPI HornBlow(int atsHornBlowIndex)
{
}

ATS_API void WINAPI DoorOpen()
{
	g_pilotlamp = false;
	g_spp.StopChime(); // ��ʂ̃`���C�����~�߂�
}

ATS_API void WINAPI DoorClose()
{
	g_ato->finishStop();
	g_pilotlamp = true;
}

ATS_API void WINAPI SetSignal(int signal)
{
}

ATS_API void WINAPI SetBeaconData(ATS_BEACONDATA beaconData){

	switch(beaconData.Type)
	{
	case ATS_BEACON_S: // S�����O
		g_atsp.Exit();
		if(g_speed != 0){g_atssn.PassedLong(beaconData.Signal);} // �w�W�����v�����O����
		break;
	case ATS_BEACON_SN: // SN����
		g_atsp.Exit();
		g_atssn.PassedShort(beaconData.Signal);
		break;
	case ATS_BEACON_P: // ��~�M��
		g_atssn.TurnOff();
		g_atsp.PassedSig(beaconData.Distance);
		break;
	case ATS_BEACON_EMG: // ����(���)
		g_atssn.TurnOff();
		g_atsp.PassedStopEmg(beaconData.Distance);
		break;
	case ATS_BEACON_SVC: // ����(��p)
		g_atssn.TurnOff();
		g_atsp.PassedStopSvc(beaconData.Distance);
		break;
	case ATS_BEACON_SPDLIM: // ����푬�x����
		g_atssn.TurnOff();
		g_atsp.PassedSpeedLim(beaconData.Optional % 1000, beaconData.Optional / 1000);
		break;
	case ATS_BEACON_SPDMAX: // �ō����x
		g_atssn.TurnOff();
		g_atsp.PassedSpeedMax(beaconData.Optional);
		break;
	case ATS_BEACON_SPP: // ��ԉw�ʉߖh�~���u
        g_spp.Receive(beaconData.Optional);
	}
}