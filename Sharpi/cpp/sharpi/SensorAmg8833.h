#pragma once

#include <mutex>
#include <chrono>
#include <thread>
#include <string>

#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"

#include "Sensor.h"
#include "DevI2c.h"

using namespace std;
using namespace chrono;

class SensorAmg8833 : Sensor
{
private:
	static string description;

private:
	static const string I2CDEVICE;
	
	string _i2cDevice = I2CDEVICE;
	DevI2c* _devI2c = NULL;
	uint8_t _i2cAddress;
	SkImageInfo _skImageInfo;

public:
	enum InterruptMode : uint8_t
	{
		None = 0x00,
		Absolute = 0x01,
		Difference = 0x02
	};

	SkBitmap skBitmap;

public:		
	SensorAmg8833(uint8_t i2cAddress);
	SensorAmg8833(uint8_t i2cAddress, string i2cDevice);
	~SensorAmg8833();
	
	string GetDescription();
	const char* GetDescriptionC();

	void PowerOn();  // 5.6 mA
	void PowerOff(); // 0.55 mA

	void ClearStatus(bool overflow, bool interrupt);
	void GetStatus(bool* overflow, bool* interrupt);

	void SetMovingAverageMode(bool on);
	void SetFrameRate(bool high);

	void SetInterrupt(InterruptMode interruptMode);
	void SetInterrupt(InterruptMode interruptMode, float highTemp, float lowTemp);
	void SetInterrupt(InterruptMode interruptMode, float highTemp, float lowTemp, float hysteresis);

	void ReadInterrupts(uint8_t values[8][8]);

	void ReadThermistorShort(short* value);
	void ReadThermistorFloat(float* value);

	void ReadTemperaturesShort(short values[8][8]);
	void ReadTemperaturesFloat(float values[8][8]);
	
	void UpdateTemperaturesBitmap(float minTemp, float maxTemp);

public:
	class Regs
	{
	public:
		/// <summary>
		/// Power control register
		/// </summary>
		class PCLT_RW
		{
		public:
			static const uint8_t addr = 0x00;
			class OperatingMode
			{
			public:
				enum : uint8_t
				{
					/// <summary>
					/// After changing from sleep to normal mode, wait 50msec -> Write Initial reset -> Wait more than 2 frames -> Write flag reset.
					/// Wait more than 2 frames before reading thermistor and temperature data.
					/// </summary>
					normal = 0x00,
					sleep = 0x10
				};
			};
		};

		/// <summary>
		/// Reset register
		/// </summary>
		class RST_W
		{
		public:
			static const uint8_t addr = 0x01;
			class ResetType
			{
			public:
				enum : uint8_t
				{
					/// <summary>
					/// Clears all status registers and interrupt tables
					/// </summary>
					flag = 0x30,
					/// <summary>
					/// Clears all status registers and interrupt tables, reads adjustment value from nonvolatile memory.
					/// Do not use except for changing operation mode from sleep to normal mode
					/// </summary>
					initial = 0x3F
				};
			};
		};

		/// <summary>
		/// Frame rate register
		/// </summary>
		class FPSC_RW
		{
		public:
			static const uint8_t addr = 0x02;
			class FramesPerSecond
			{
			public:
				enum : uint8_t
				{
					fps1 = 0b00000001,
					fps10 = 0b00000000,
				};
			};
		};

		/// <summary>
		/// Interrupt control register
		/// </summary>
		class INTC_RW
		{
		public:
			static const uint8_t addr = 0x03;
			class InterruptMode
			{
			public:
				enum : uint8_t
				{
					absolute = 0b00000010,
					difference = 0b00000000
				};
			};
			class InterruptEnable
			{
			public:
				enum : uint8_t
				{
					active = 0b00000001,
					inactive = 0b00000000
				};
			};
		};

		/// <summary>
		/// Status register, overflow and interrupt flag
		/// </summary>
		class STAT_R
		{
		public:
			static const uint8_t addr = 0x04;
			class StatusBits
			{
			public:
				enum : uint8_t
				{
					TempOutOverflow = 0x04,
					TempOutInterrupt = 0x02
				};
			};
		};

		/// <summary>
		/// Status clear register
		/// </summary>
		class SCLR_W
		{
		public:
			static const uint8_t addr = 0x05;
			class ClearBits
			{
			public:
				enum : uint8_t
				{
					overflow = 0x04,
					interrupt = 0x02
				};
			};
		};

		/// <summary>
		/// Average register
		/// </summary>
		class AVE_RW
		{
		public:
			static const uint8_t addr = 0x07;
			class MovingAverageMode
			{
			public:
				enum : uint8_t
				{
					off = 0x00,
					on = 0x20
				};
			};
		};

		/// <summary>
		/// Interrupt level register high limit [7:0]
		/// </summary>
		class INTHL_RW
		{
		public:
			static const uint8_t addr = 0x08;
		};

		/// <summary>
		/// Interrupt level register high [11:8]
		/// </summary>
		class INTHH_RW
		{
		public:
			static const uint8_t addr = 0x09;
		};

		/// <summary>
		/// Interrupt level register low limit [7:0]
		/// </summary>
		class INTLL_RW
		{
		public:
			static const uint8_t addr = 0x0A;
		};

		/// <summary>
		/// Interrupt level register low limit [11:8]
		/// </summary>
		class INTLH_RW
		{
		public:
			static const uint8_t addr = 0x0B;
		};

		/// <summary>
		/// Interrupt level register hysteresis [7:0]
		/// </summary>
		class IHYSL_RW
		{
		public:
			static const uint8_t addr = 0x0C;
		};

		/// <summary>
		/// Interrupt level register hysteresis [7:0]
		/// </summary>
		class IHYSH_RW
		{
		public:
			static const uint8_t addr = 0x0D;
		};

		/// <summary>
		/// Thermistor register
		/// </summary>
		class TTH_R
		{
		public:
			static const uint8_t addr = 0x0E;
			static const uint16_t size = 0x02;
		};

		/// <summary>
		/// Interrupt register
		/// </summary>
		class INT_R
		{
		public:
			static const uint8_t addr = 0x10;
			static const uint16_t size = 0x08;
		};

		/// <summary>
		/// Temperature register
		/// </summary>
		class TO_R
		{
		public:
			static const uint8_t addr = 0x80;
			static const uint16_t size = 0x80;
		};
	};

private:
	void ConvThermistorRegToShort(short* val, uint8_t regVal[2]);
	void ConvTemperatureRegToShort(short* val, uint8_t regVal[2]);
	void ConvTemperatureShortToReg(uint8_t regVal[2], short val);
	
	short ConvFloatToShort(float value);
	float ConvShortToFloat(short value);



	

};
