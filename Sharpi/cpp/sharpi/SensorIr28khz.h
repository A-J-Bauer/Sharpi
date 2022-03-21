#pragma once

#include <mutex>
#include <chrono>
#include <functional>
#include <thread>
#include <string>
#include <atomic>
#include <thread>
#include <mutex>

#include <string.h>

#include "DevGpio.h"

#include "Sensor.h"

using namespace std;
using namespace chrono;

class SensorIr28khz : Sensor
{
private:
	static string description;

	static const string CHIP;	

	static const uint32_t NEC_DELTA;
	static const uint32_t NEC_BURST;
	static const uint32_t NEC_ZERO;
	static const uint32_t NEC_ONE;
	static const uint32_t NEC_LEADINGBURST;
	static const uint32_t NEC_LEADINGSPACENEW;
	static const uint32_t NEC_LEADINGSPACEREPEAT;

	static const int NUMSAMPLES = 256;	
	
	function<void(uint16_t address, uint16_t command)> _callback_nec = NULL;	

	thread worker;
	atomic<bool> stop = false;

	int _pin;
	bool _activeLow;

public:
	SensorIr28khz(int pin, bool activeLow, function<void(uint16_t, uint16_t)> callback_nec);
	~SensorIr28khz();

	string GetDescription();
	const char* GetDescriptionC();

	void PowerOn();
	void PowerOff();

private:
	uint32_t nanodiff(gpiod_line_event e, gpiod_line_event s) { return (e.ts.tv_sec - s.ts.tv_sec) * 1000000000 + e.ts.tv_nsec - s.ts.tv_nsec; }
	bool equal(uint32_t n1, uint32_t n2) { return labs((long)n1 - (long)n2) < NEC_DELTA; };


	void work(int pin, bool activeLow);

};
