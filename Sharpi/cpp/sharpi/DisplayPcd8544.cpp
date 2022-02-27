#include "DisplayPcd8544.h"

string DisplayPcd8544::description = {
"-----------------------------------------\n"
"PCD8544 also known as Nokia screen       \n"
"                                         \n"
"config:                                  \n"
"                                         \n"
"  edit /boot/config.txt                  \n"
"  dtparam=spi=on                         \n"
"                                         \n"
"wiring:                                  \n"
"                                         \n"
"       rpi physical pins                 \n"
"                                         \n"
"             1   2                       \n"
"             3   4                       \n"
"             5   6  -- gnd               \n"
"             7   8                       \n"
"     gnd --  9   10                      \n"
"            11   12                      \n"
"            13   14 -- gnd               \n"
"     vcc -- 15   16 -- dc                \n"
"            17   18 -- rst               \n"
"     din -- 19   20                      \n"
"            21   22                      \n"
"     clk -- 23   24 -- ce                \n"
"     gnd -- 25   26                      \n"
"            27   28                      \n"
"            29   30                      \n"
"            31   32                      \n"
"            33   34                      \n"
"            35   36                      \n"
"            37   38                      \n"
"            39   40                      \n"
"-----------------------------------------\n"
};

string DisplayPcd8544::GetDescription() {
	
	return description;
};

const char* DisplayPcd8544::GetDescriptionC()
{
	return description.c_str();
}

void DisplayPcd8544::SendCmd(PCD8544_CMD cmd, unsigned char cmdExt = 0x00)
{
	DevGpio::DigitalWrite(_dataPin, false);
	unsigned char buffer[1] = { (unsigned char)(cmd | cmdExt) };
	devSpi->Write(buffer, 1);
}

void DisplayPcd8544::SendData(unsigned char* buffer, unsigned int len)
{
	DevGpio::DigitalWrite(_dataPin, true);
	devSpi->Write(buffer, len);
}


DisplayPcd8544::DisplayPcd8544() 
{
	devSpi = new DevSpi(_spiDevice, 0, 8, _spiSpeedHz, 0);
	devSpi->Open();

	if (_powerPin == UNDEFINED_PIN)
	{
		_powerPin = POWERPIN;
	}

	if (_dataPin == UNDEFINED_PIN)
	{
		_dataPin = DATAPIN;
	}

	if (_resetPin == UNDEFINED_PIN)
	{
		_resetPin = RESETPIN;
	}

	skImageInfo = SkImageInfo::Make(84, 48, SkColorType::kGray_8_SkColorType, SkAlphaType::kPremul_SkAlphaType);
	skBitmap.allocPixels(skImageInfo);

	memset(_blank, 0, _bufferSize);
}

DisplayPcd8544::DisplayPcd8544(string spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin) : DisplayPcd8544()
{
	_spiDevice = spiDevice;
	_spiSpeedHz = spiSpeedHz;
	_powerPin = powerPin;
	_dataPin = dataPin;
	_resetPin = resetPin;
}

DisplayPcd8544::~DisplayPcd8544()
{
	
	PowerOff();
	devSpi->Close();
	delete devSpi;
}




void DisplayPcd8544::PowerOn()
{
	if (!_isOn)
	{
		DevGpio::SetPinMode(_powerPin, DevGpio::OUTPUT);
		DevGpio::SetPinMode(_dataPin, DevGpio::OUTPUT);
		DevGpio::SetPinMode(_resetPin, DevGpio::OUTPUT);

		DevGpio::DigitalWrite(_resetPin, DevGpio::HIGH);
		usleep(20000);
		DevGpio::DigitalWrite(_powerPin, DevGpio::HIGH);
		usleep(20000);		
		DevGpio::DigitalWrite(_resetPin, DevGpio::LOW);
		usleep(20000);
		DevGpio::DigitalWrite(_resetPin, DevGpio::HIGH);
		usleep(20000);

		SendCmd(PCD8544_CMD::FunctionSet, 0x01);
		SendCmd(PCD8544_CMD::SetBias, 0x07);
		SendCmd(PCD8544_CMD::FunctionSet);

		SendCmd(PCD8544_CMD::FunctionSet, 0x01);
		SendCmd(PCD8544_CMD::SetVop, 0x07);
		SendCmd(PCD8544_CMD::FunctionSet);

		SendCmd(PCD8544_CMD::DisplayControl, 0x04);

		SendCmd(PCD8544_CMD::SetXAddr);
		SendCmd(PCD8544_CMD::SetYAddr);

		SendData(_blank, _bufferSize);

		_isOn = true;		
	}
}

void DisplayPcd8544::PowerOff()
{
	if (_isOn)
	{
		// for simplicity we just cut power (there is a sleep/power down mode, see manual page 15 specifics)
		// if this ugly display is ever used a lot then we would need to check if _powerPin was set to UNDEFINED_PIN (-1)
		// in the constructor and do a sleep rather than cutting power
		SendData(_blank, _bufferSize);

		DevGpio::DigitalWrite(_resetPin, DevGpio::LOW);
		DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);
		DevGpio::DigitalWrite(_powerPin, DevGpio::LOW);

		_isOn = false;
	}
}

bool DisplayPcd8544::IsOn()
{
	return _isOn;
}



void DisplayPcd8544::Update()
{
	/*
	 pcd8544
	 vertical addressing

	0  6  ...  83
	1  7
	2  8
	3  .
	4  .
	5  11 ... 503

	 */

	SendCmd(PCD8544_CMD::SetYAddr);
	SendCmd(PCD8544_CMD::SetXAddr);
	
	memcpy(_gray, skBitmap.getPixels(), 84 * 48);
	memset(_blwt, 0, _bufferSize);

	int k = 0;
	int l = 0;
	for (int i = 0; i < 84; i++)
	{
		for (int j = 0; j < 48; j++)
		{
			_blwt[k] = (unsigned char)(_blwt[k] | (_gray[j * 84 + i] == 0 ? 1 << l : 0));

			l++;
			if (l % 8 == 0)
			{
				k++;
				l = 0;
			}
		}
	}
	
	SendData(_blwt, _bufferSize);
}
