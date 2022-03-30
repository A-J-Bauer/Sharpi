#pragma once

#include <thread>
#include <chrono>
#include <string>
#include <cstring>
#include <atomic>
#include <tuple>
#include <map>
#include <iostream>

#include <gpiod.h>

using namespace std;
using namespace std::this_thread;
using namespace chrono;

class DevGpio
{
public:
	struct Seq
	{
		int64_t nano;
		uint8_t value;
	};

	static thread sequence[40];
	static atomic<bool> running[40];

public:
	static const int MIN_PIN = 0;
	static const int MAX_PIN = 40;

	static const bool HIGH = true;
	static const bool LOW = false;

	static const int INPUT = 0;
	static const int OUTPUT = 1;
	static const int INPUT_PULL_UP = 2;
	static const int INPUT_PULL_DOWN = 3;
	static const int INPUT_UNKNOWN = 4;

private:
	static void sequencer(int pin, Seq* seq, int length);

public:
	static const char* gpiod_version;;

	static bool SetPinMode(int pin, int mode);
	static int GetPinMode(int pin);

	static void DigitalWrite(int pin, bool value);
	static bool DigitalWriteSequence(int pin, Seq* seq, int length);
	static bool DigitalRead(int pin);

	static int J8Gpio(int pin);
	static string J8Info(int pin);

	static string J8PinOut();
	static const char* J8PinOutC();

private:
	DevGpio() {}; // disallow instantiation

	static bool Open(string device);
	static void Close();
	static bool Check(int line);

	static gpiod_line* lines[40];
	static gpiod_chip* chip;
	static const char* consumer;

	static map<int, std::tuple<int, std::string>> J8;
	static string j8PinOut;

};