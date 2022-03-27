#include "DisplaySsd1351.h"

string DisplaySsd1351::description = 
"-----------------------------------------\n"
"SSD1351 Oled 128x128 screen              \n"
"                                         \n"
"config:                                  \n"
"                                         \n"
"  edit /boot/config.txt                  \n"
"  dtparam=spi=on                         \n"
"  dtoverlay=spi1-1cs (optional for SPI 2)\n"
"                                         \n"
"wiring:                                  \n"
"                                         \n"
"       rpi physical pins                 \n"
"                                         \n"
"  _0 for SPI 0 and _1 for SPI 1          \n"
"                                         \n"
"     3.3v -- 1   2  -- 5v                \n"
"             3   4  -- 5v                \n"
"             5   6  -- gnd               \n"
"             7   8                       \n"
"     gnd --  9   10                      \n"
"            11   12 -- cs_1              \n"
"   rst_1 -- 13   14 -- gnd               \n"
"    dc_1 -- 15   16 -- dc_0              \n"
"            17   18 -- rst_0             \n"
"  din_0 --  19   20                      \n"
"            21   22                      \n"
"   clk_0 -- 23   24 -- cs_0              \n"
"     gnd -- 25   26                      \n"
"            27   28                      \n"
"            29   30                      \n"
"            31   32                      \n"
"            33   34                      \n"
"            35   36                      \n"
"            37   38 -- din_1             \n"
"            39   40 -- clk_1             \n"
"-----------------------------------------\n"
;

string DisplaySsd1351::GetDescription()
{
	return description;
};

const char* DisplaySsd1351::GetDescriptionC()
{
	return description.c_str();
}

void DisplaySsd1351::Send(SSD1351_CMD cmd, unsigned char* data = NULL, size_t size = 0)
{
	DevGpio::DigitalWrite(_dataPin, false);
	unsigned char command[1] = { (unsigned char)cmd };
	_devSpi->Write(command, 1);

	if (!data)
	{
		return;
	}

	DevGpio::DigitalWrite(_dataPin, true);	
	_devSpi->Write(data, size);
}


const string DisplaySsd1351::SPIDEVICE = "/dev/spidev0.0";
const int DisplaySsd1351::UNDEFINED_PIN = -1;
const int DisplaySsd1351::SPISPEEDHZ = 24000000;
const int DisplaySsd1351::DATAPIN[2] = { 16,15 }; // low command, high data
const int DisplaySsd1351::RESETPIN[2] = { 18, 13 };


/// <summary>
/// Default constructor
/// spiDevice "/dev/spidev0.0"
/// rotation 0
/// SPI0 pins see GetDescription
/// </summary>
DisplaySsd1351::DisplaySsd1351()
	: DisplaySsd1351(SPIDEVICE, 0)
{	
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="spiDevice">"/dev/spidev0.0" or "/dev/spidev1.0"</param>
/// <param name="rotation">initial rotation (0,..,3)</param>
DisplaySsd1351::DisplaySsd1351(string spiDevice, unsigned char rotation) 
	: DisplaySsd1351(spiDevice, rotation, SPISPEEDHZ, UNDEFINED_PIN, UNDEFINED_PIN)
{	
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="spiDevice"></param>
/// <param name="rotation"></param>
/// <param name="spiSpeedHz"></param>
/// <param name="dataPin"></param>
/// <param name="resetPin"></param>
DisplaySsd1351::DisplaySsd1351(string spiDevice, unsigned char rotation, int spiSpeedHz, int dataPin, int resetPin) 	
{		
	_spiDevice = spiDevice == "/dev/spidev1.0" ? spiDevice : "/dev/spidev0.0";
	unsigned char busidx = _spiDevice == "/dev/spidev1.0" ? 1 : 0;

	_rotation = rotation;
	_spiSpeedHz = spiSpeedHz;
		
	_dataPin = dataPin == UNDEFINED_PIN ? DATAPIN[busidx] : dataPin;
	_resetPin = resetPin == UNDEFINED_PIN ? RESETPIN[busidx] : resetPin;

	_devSpi = new DevSpi(_spiDevice, 0, 8, _spiSpeedHz, 0);
	_devSpi->Open();

	_skImageInfo = SkImageInfo::Make(128, 128, SkColorType::kRGB_565_SkColorType, SkAlphaType::kPremul_SkAlphaType);
	skBitmap.allocPixels(_skImageInfo);
	
	switch (_rotation % 4)
	{
	case 0:
		remap |= 0b00010000;
		startLine = 128;
		break;
	case 1:
		remap |= 0b00010011;
		startLine = 128;
		break;
	case 2:
		remap |= 0b00000010;
		startLine = 0;
		break;
	case 3:
		remap |= 0b00000001;
		startLine = 0;
		break;
	}

}

DisplaySsd1351::~DisplaySsd1351()
{
	PowerOff();
	_devSpi->Close();
	delete _devSpi;
}

void DisplaySsd1351::PowerOn()
{
	if (!_isOn)
	{
		if (!_initialized)
		{			
			DevGpio::SetPinMode(_dataPin, DevGpio::OUTPUT);
			DevGpio::SetPinMode(_resetPin, DevGpio::OUTPUT);

			DevGpio::DigitalWrite(_resetPin, DevGpio::LOW);
			usleep(200000);
			DevGpio::DigitalWrite(_resetPin, DevGpio::HIGH);
			usleep(20000);


			_cmddata[0] = 0x12; 
			Send(SSD1351_CMD::SetCommandLocks, _cmddata, 1);
			_cmddata[0] = 0xB1;
			Send(SSD1351_CMD::SetCommandLocks, _cmddata, 1);
			Send(SSD1351_CMD::SleepIn);
			_cmddata[0] = 0xFF;
			Send(SSD1351_CMD::SetClockDiv, _cmddata, 1);
			_cmddata[0] = 127;
			Send(SSD1351_CMD::SetMultiplexorRatio, _cmddata, 1);
			_cmddata[0] = 0x00;
			Send(SSD1351_CMD::SetDisplayOffset, _cmddata, 1);
			_cmddata[0] = 0x01;
			Send(SSD1351_CMD::SelectFunction, _cmddata, 1);
			_cmddata[0] = 0x32;
			Send(SSD1351_CMD::SetPrecharge, _cmddata, 1);
			_cmddata[0] = 0x05;
			Send(SSD1351_CMD::SetDeselectVoltageLevel, _cmddata, 1);
			Send(SSD1351_CMD::SetNormalDisplay);
			_cmddata[0] = 0xD0; _cmddata[1] = 0xD0; ; _cmddata[2] = 0xD0;
			Send(SSD1351_CMD::SetContrastABC, _cmddata, 3);
			_cmddata[0] = 0b1111;
			Send(SSD1351_CMD::SetContrastMasterCurrent, _cmddata, 1);
			_cmddata[0] = 0x01;
			Send(SSD1351_CMD::SetPrecharge2, _cmddata, 1);
			Send(SSD1351_CMD::SleepOut);
			memset(_cmddata, 0, 128 * 128 * 2);
			Send(SSD1351_CMD::WriteRam, _cmddata, 128 * 128 * 2);
			_cmddata[0] = 0; _cmddata[1] = 127;
			Send(SSD1351_CMD::SetColumn, _cmddata, 2);
			Send(SSD1351_CMD::SetRow, _cmddata, 2);
			_cmddata[0] = remap;
			Send(SSD1351_CMD::SetRemap, _cmddata, 1);
			_cmddata[0] = startLine;
			Send(SSD1351_CMD::SetStartLine, _cmddata, 1);

			_initialized = true;
		}
		else
		{
			Send(SSD1351_CMD::SelectFunction, new unsigned char[1] { 0x01 }, 1);
			usleep(10000);
			Send(SSD1351_CMD::SleepOut);
		}

		_isOn = true;
	}
}


void DisplaySsd1351::PowerOff()
{
	if (_isOn)
	{
		// we have no power pin so sleep < 1mA current draw
		Send(SSD1351_CMD::SleepIn);
		Send(SSD1351_CMD::SelectFunction, new unsigned char[1]{ 0x00 }, 1);

		_isOn = false;
	}
}

bool DisplaySsd1351::IsOn()
{
	return _isOn;
}

void DisplaySsd1351::Update()
{
	memcpy(_565, skBitmap.getPixels(), 128 * 128 * 2);
		
	uint8_t bucket;
	//low byte of 565 16-bit expected first            
	for (int i = 0; i < 128 * 128; i++)
	{
		bucket = _565[i * 2];
		_565[i * 2] = _565[i * 2 + 1];
		_565[i * 2 + 1] = bucket;
	}

	Send(SSD1351_CMD::WriteRam, _565, 128 * 128 * 2);
}