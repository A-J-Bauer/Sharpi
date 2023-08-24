#include "PotDs3502.h"

string PotDs3502::description =
"----------------------------------------------\n"
" Maxim DS3502                                 \n"
"                                              \n"
" The DS3502 is a 7-bit, nonvolatile (NV)      \n"
" digital potentiometer (POT) featuring an     \n"
" output voltage range of up to 15.5V.         \n"
" Programming is accomplished by an I2C-       \n"
" compatible interface, which can operate at   \n"
" speeds of up to 400kHz.External voltages are \n"
" applied at the RL and RH inputs to define the\n"
" lowest and highest potentiometer outputs.    \n"
" (10 uSOP package)                            \n"
"                                              \n"
" I2c slave address adjustable via A0 and A1   \n"
"                                              \n"
" A convenient breakout board is available     \n"
" from Adafruit.                               \n"
"                                              \n"
"config:                                       \n"
"                                              \n"
"  edit /boot/config.txt                       \n"
"  dtparam=i2c_arm=on,i2c_arm_baudrate=400000  \n"
"                                              \n"
"wiring:                                       \n"
"                                              \n"
"       rpi physical pins                      \n"
"                                              \n"
"     vin --  1   2                            \n"
"     sda --  3   4                            \n"
"     scl --  5   6                            \n"
"             7   8                            \n"
"     gnd --  9   10         ----------        \n"
"   (int) -- 11   12   sda -| +        |- scl  \n"
"            13   14   gnd -|          |- v+   \n"
"            15   16   vcc -|  ds3502  |- rl   \n"
"            17   18    a1 -|          |- rw   \n"
"            19   20    a0 -|          |- rh   \n"
"            21   22         ----------        \n"
"            23   24                           \n"
"            25   26                           \n"
"            27   28                           \n"
"            29   30                           \n"
"            31   32                           \n"
"            33   34                           \n"
"            35   36                           \n"
"            37   38                           \n"
"            39   40                           \n"
"                                              \n"
"----------------------------------------------\n"
;

const string PotDs3502::I2CDEVICE = "/dev/i2c-1";

PotDs3502::PotDs3502(uint8_t i2cAddress)
	: PotDs3502::PotDs3502(i2cAddress, I2CDEVICE)
{
}

PotDs3502::PotDs3502(uint8_t i2cAddress, string i2cDevice)
{
	_i2cAddress = i2cAddress;
	_i2cDevice = i2cDevice;
}

PotDs3502::~PotDs3502()
{
}

string PotDs3502::GetDescription()
{
	return description;
};

const char* PotDs3502::GetDescriptionC()
{
	return description.c_str();
}

void PotDs3502::PowerOn()
{
	if (_devI2c == NULL)
	{
		_devI2c = new DevI2c(_i2cDevice, _i2cAddress);
		if (!_devI2c->Open())
		{
			delete _devI2c;
			_devI2c = NULL;
		}

		_devI2c->WriteRegU8(Regs::CR::addr, Regs::CR::Modes::Mode_1_WR_ONLY);
	}
}

void PotDs3502::PowerOff()
{
	if (_devI2c != NULL)
	{
		_devI2c->Close();
		delete _devI2c;
		_devI2c = NULL;
	}
}


uint8_t PotDs3502::GetWiper(void)
{
	if (_devI2c == NULL) { return 0; };

	uint8_t value;
	_devI2c->ReadRegU8(Regs::WR::addr, &value);
	return value;
}

void PotDs3502::SetWiper(uint8_t value)
{
	if (_devI2c == NULL) { return; };

	if (value > 127) { return; }

	_devI2c->WriteRegU8(Regs::WR::addr, value);
}

void PotDs3502::SetWiperPersistent(uint8_t value)
{
	if (_devI2c == NULL) { return; };

	if (value > 127) { return; }

	_devI2c->WriteRegU8(Regs::CR::addr, Regs::CR::Modes::Mode_0_WR_IVR);
	_devI2c->WriteRegU8(Regs::WR::addr, value);
	usleep(20000); // max EEPROM write time 20ms
	_devI2c->WriteRegU8(Regs::CR::addr, Regs::CR::Modes::Mode_1_WR_ONLY);
}