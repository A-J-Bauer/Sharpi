#include <cstring>
#include <unistd.h>
#include <iostream>
#include <string>
#include <locale>
#include <codecvt>

#include "DisplayTm1637.h"

string DisplayTm1637::description = "";

string DisplayTm1637::GetDescription() 
{
	if (description == "")
	{
		description =
			"TM1637 7 segment LCD Titan Micro Elec.  \n"
			"                                         \n"
			"config:                                  \n"
			"                                         \n"
			"  nothing to do since a custom protocol  \n"
			"  is implemented by this device (!i2c)   \n"
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
			"     vcc -- 15   16 -- dio               \n"
			"            17   18 -- clk               \n"
			"            19   20                      \n"
			"            21   22                      \n"
			"            23   24                      \n"
			"     gnd -- 25   26                      \n"
			"            27   28                      \n"
			"            29   30                      \n"
			"            31   32                      \n"
			"            33   34                      \n"
			"            35   36                      \n"
			"            37   38                      \n"
			"            39   40                      \n"
			"                                         \n"
			"supported characters:                    \n";

		u16string u16sup = u"  ";
		for (auto it = CharToData.begin(); it != CharToData.end(); it++)
		{
			u16sup += it->first;
		}

		string supported = ":" + wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(u16sup);

		description += supported + "\n";
	}
		
	return description;
};

const char* DisplayTm1637::GetDescriptionC()
{
	GetDescription();
	return description.c_str();
}

map<char16_t, uint8_t> DisplayTm1637::CharToData =
{
	//      XGFEDCBA
	{'-', 0b01000000},
	{'0', 0b00111111},
	{'1', 0b00000110},
	{'2', 0b01011011},
	{'3', 0b01001111},
	{'4', 0b01100110},
	{'5', 0b01101101},
	{'6', 0b01111101},
	{'7', 0b00000111},
	{'8', 0b01111111},
	{'9', 0b01101111},
	{'A', 0b01110111},
	{'b', 0b01111100},
	{'C', 0b00111001},
	{'d', 0b01011110},
	{'E', 0b01111001},
	{'F', 0b01110001},
	{'r', 0b01010000},
	{'O', 0b00111111},
	{'L', 0b00111000},
	{'P', 0b01110011},
	{'S', 0b00111111},
	{u'\u00B0', 0b01100011},
   {'\'', 0b00100000},
	{'"', 0b00100010},
	//      XGFEDCBA
};
//             A
//            ---
//         F |   | B
//            -G-
//         E |   | C
//            ---
//             D




DisplayTm1637::DisplayTm1637()
{
	if (_powerPin == UNDEFINED_PIN)
	{
		_powerPin = POWERPIN;
	}

	if (_dataPin == UNDEFINED_PIN)
	{
		_dataPin = DATAPIN;
	}

	if (_clockPin == UNDEFINED_PIN)
	{
		_clockPin = CLOCKPIN;
	}

}

// set powerPin to UNUSED_PIN (-1) if power pin is used in wiring
DisplayTm1637::DisplayTm1637(int powerPin, int dataPin, int clockPin) : DisplayTm1637()
{	
	_powerPin = powerPin;
	_dataPin = dataPin;
	_clockPin = clockPin;
}

DisplayTm1637::~DisplayTm1637()
{
	PowerOff();
}


void DisplayTm1637::PowerOn()
{
	if (!_isOn)
	{
		if (_powerPin != UNUSED_PIN) 
		{
			DevGpio::SetPinMode(_powerPin, DevGpio::OUTPUT);
			DevGpio::DigitalWrite(_powerPin, DevGpio::HIGH);
		}

		DevGpio::SetPinMode(_dataPin, DevGpio::OUTPUT);
		DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);

		DevGpio::SetPinMode(_clockPin, DevGpio::OUTPUT);
		DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);

		_isOn = true;
	}
}

void DisplayTm1637::PowerOff()
{
	if (_isOn)
	{
		if (_powerPin != UNUSED_PIN) 
		{ 
			DevGpio::DigitalWrite(_powerPin, DevGpio::LOW);
			DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);
			DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);
		} 
		else
		{
			_brightnessSaved = _brightness;
			Update();
		}		

		_isOn = false;
	}
}

bool DisplayTm1637::IsOn()
{
	return _isOn;
}

void DisplayTm1637::Update() 
{
	datar[0] = data[3];
	datar[1] = data[2] | (data[1] & (1 << SH_DP));
	datar[2] = data[1] & (0xFF ^ (1 << SH_DP));
	datar[3] = data[0];

	uint8_t fr, to;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			fr = 1 << shrot_fr[j];
			to = 1 << shrot_to[j];

			if (((datar[i] & fr) >> shrot_fr[j]) ^ ((datar[i] & to) >> shrot_to[j]))
			{
				datar[i] ^= fr;
				datar[i] ^= to;
			}
		}
	}

	SetSegments(_rotate ? datar : data);
}

void DisplayTm1637::SetBrightness(int zerotofour)
{
	switch (zerotofour)
	{
	case 0:
		_brightness = 0b00000000;
		break;
	case 1:
		_brightness = 0b00001000;
		break;
	case 2:
		_brightness = 0b00001001;
		break;
	case 3:
		_brightness = 0b00001010;
		break;
	case 4:
		_brightness = 0b00001100;
		break;
	default:
		_brightness = 0b00000000;
		break;
	}	
}

void DisplayTm1637::SetRotation(bool rotate)
{
	_rotate = rotate;
}

void DisplayTm1637::SetText(string text)
{
	u16string textUtf16 = wstring_convert<codecvt_utf8_utf16<char16_t>, char16_t>{}.from_bytes(text.data());
	size_t l = textUtf16.size();

	if (l < 1 || l > 5)
	{
		textUtf16 = u"Err ";
		l = 4;
	}

	memset(data, 0, 4);

	int i = 0;
	int j = 0;
	for (char16_t c : textUtf16)
	{
		//cout << std::hex << std::showbase << c << '\n';
			
		if (CharToData.find(c) != CharToData.end())
		{
			data[j] = CharToData.at(c);
			j++;
		}
		else if (l == 5 && i == 2 && textUtf16.at(i) == ':')
		{
			data[1] |= 1 << SH_DP;
		}
		else
		{
			data[j] = 0;
			j++;
		}
		i++;
	}
}



void DisplayTm1637::SetSegments(const uint8_t seg[], uint8_t pos, uint8_t length)
{	
	StartWrite();
	WriteByte(COMM1);
	StopWrite();

	StartWrite();
	WriteByte(COMM2 + (pos & 0x03));

	for (int i = 0; i < length; i++)
	{
		WriteByte(seg[i]);
	}

	StopWrite();

	StartWrite();
	WriteByte(COMM3 + (_brightness & 0x0f));
	StopWrite();
}

void DisplayTm1637::StartWrite()
{
	DevGpio::DigitalWrite(_clockPin, DevGpio::HIGH);
	DevGpio::DigitalWrite(_dataPin, DevGpio::HIGH);
	usleep(_bitDelay);
	DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);	
}

void DisplayTm1637::StopWrite()
{
	DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);
	DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);
	usleep(_bitDelay);
	DevGpio::DigitalWrite(_clockPin, DevGpio::HIGH);
	DevGpio::DigitalWrite(_dataPin, DevGpio::HIGH);
}

bool DisplayTm1637::WriteByte(uint8_t b)
{
	uint8_t data = b;

	// 8 bits
	for (uint8_t i = 0; i < 8; i++) {		
		DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);
		usleep(_bitDelay);
		
		if (data & 0x01)
			DevGpio::DigitalWrite(_dataPin, DevGpio::HIGH);
		else
			DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);

		usleep(_bitDelay);		
		DevGpio::DigitalWrite(_clockPin, DevGpio::HIGH);
		usleep(_bitDelay);
		data = data >> 1;
	}

	// wait ack
	
	DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);
	DevGpio::DigitalWrite(_dataPin, DevGpio::HIGH);

	usleep(_bitDelay);
	
	DevGpio::DigitalWrite(_clockPin, DevGpio::HIGH);

	usleep(_bitDelay);

	DevGpio::SetPinMode(_dataPin, DevGpio::INPUT);
	bool ack = !DevGpio::DigitalRead(_dataPin);
	if (ack)
	{
		DevGpio::SetPinMode(_dataPin, DevGpio::OUTPUT);
		DevGpio::DigitalWrite(_dataPin, DevGpio::LOW);
	}

	usleep(_bitDelay);

	DevGpio::DigitalWrite(_clockPin, DevGpio::LOW);

	usleep(_bitDelay);

	return ack;
}

