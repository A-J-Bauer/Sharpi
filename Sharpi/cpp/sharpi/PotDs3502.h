#pragma once

#include "Pot.h"
#include "DevI2c.h"

using namespace std;

class PotDs3502 : Pot
{
private:
	static string description;

private:
	static const string I2CDEVICE;

	string _i2cDevice = I2CDEVICE;
	DevI2c* _devI2c = NULL;
	uint8_t _i2cAddress;

public:
	PotDs3502(uint8_t i2cAddress);
	PotDs3502(uint8_t i2cAddress, string i2cDevice);
	~PotDs3502();

	string GetDescription();
	const char* GetDescriptionC();

	void PowerOn();
	void PowerOff();

	uint8_t GetWiper(void);
	void SetWiper(uint8_t value);
	void SetWiperPersistent(uint8_t value); // 50000 - 200000 EEPROM writes

public:
	class Regs
	{
	public:
		/// <summary>
		/// Control Register
		/// </summary>
		class CR
		{
		public:
			static const uint8_t addr = 0x02;
			class Modes
			{
			public:
				enum : uint8_t
				{
					/// <summary>
					/// mode0: write to memory address 00h, write to both WR (Wiper Register) and IVR (Initial Value Register), read from WR
					/// mode1: write to memory address 00h, write to WR, read from WR, for writing from SRAM to EEPROM an i2c stop must occur
					/// </summary>
					Mode_0_WR_IVR = 0x00,
					Mode_1_WR_ONLY = 0x80
				};
			};
		};

		/// <summary>
		/// Wiper Register
		/// </summary>
		class WR
		{
		public:
			static const uint8_t addr = 0x00;
			
		};
	};
};