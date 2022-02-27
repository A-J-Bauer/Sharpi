#pragma once

#include <string>
#include <filesystem>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "DevGpio.h"

using namespace std;
using namespace std::filesystem;

class SysPwm
{
private:
	static const path chippath;	
	static const path exppath;
	static const path unexppath;
	static const path enblpath[2];
	static const path perdpath[2];
	static const path dutypath[2];

	static bool _created[2];
	static bool _isOn[2];
	static bool _waveSet[2];

	static int _frequency[2];
	static int _dutypercent[2];

	static unsigned long _periodnanoseconds[2];
	static unsigned long _dutynanoseconds[2];

private:
	static void SetWave(int pwm);

public:
	static string GetDescription();

	static void SetFrequency(int pwm, int frequency);
	static void SetDutyCycle(int pwm, int dutypercent);

	static void Create(int pwm);
	static void Destroy(int pwm);

	static void PowerOn(int pwm);
	static void PowerOff(int pwm);

private:
	SysPwm() {}; // disallow instantiation

public:
	static string description;


};