#include "DisplaySh1106.h"

string DisplaySh1106::description =
"----------------------------------------------\n"
"SH1106 Oled 128x64 screen (I2c)               \n"
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
"     vdd --  1   2                            \n"
"     sda --  3   4                            \n"
"     scl --  5   6                            \n"
"             7   8             -------------  \n"
"     gnd --  9   10           | |         | | \n"
"            11   12      sda -| |         | | \n"
"            13   14      sck -| |         | | \n"
"            15   16      gnd -| |         | | \n"
"            17   18      vdd -| |         | | \n"
"            19   20           | |         | | \n"
"            21   22            -------------  \n"
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

const string DisplaySh1106::I2CDEVICE = "/dev/i2c-1";

DisplaySh1106::DisplaySh1106(uint8_t i2cAddress, bool rotation, uint8_t brightness)
    : DisplaySh1106(i2cAddress, rotation, brightness, I2CDEVICE)
{
}

DisplaySh1106::DisplaySh1106(uint8_t i2cAddress, bool rotation, uint8_t brightness, string i2cDevice)
{
    _i2cAddress = i2cAddress;
    _rotation = rotation;
    _brightness = brightness;
    _i2cDevice = i2cDevice;

    _skImageInfo = SkImageInfo::Make(128, 64, SkColorType::kGray_8_SkColorType, SkAlphaType::kPremul_SkAlphaType);
    skBitmap.allocPixels(_skImageInfo);
}

DisplaySh1106::~DisplaySh1106()
{
    PowerOff();
}

string DisplaySh1106::GetDescription()
{
    return description;
}

const char* DisplaySh1106::GetDescriptionC()
{
    return description.c_str();
}

void DisplaySh1106::Clear()
{
    memset(_blwt, 0, sizeof(_blwt));

    for (int i = 0; i < SH_PAGES; i++)
    {
        _devI2c->WriteRegU8(0x00, 0xB0 + i); // command, page number SH1106 [12]
        _devI2c->WriteRegU8(0x40, &_blwt[i * SH_WIDTH], SH_WIDTH); // data, page data
    }
}

// 3.16 mA
void DisplaySh1106::PowerOn()
{
    if (_devI2c == NULL)
    {
        _devI2c = new DevI2c(_i2cDevice, _i2cAddress);
        if (_devI2c->Open())
        {
			if (!_isOn)
			{
				if (!_initialized)
				{
                    uint8_t init[] = {                        
                        0x00,                                   // [01] 0x00-0x0F set column address lower 4 bits
                        0x10,                                   // [02] 0x10-0x1F set column address higher 4 bits
                        0x32,                                   // [03] 0x30-0x33 set pump voltage value
                        0x40,                                   // [04] set display start line 0x40-0x7F
                        0x81, _brightness,                      // [05] set contrast control register 0x00-0xFF
                        (uint8_t)(_rotation ? 0xA1 : 0xA0),     // [06] set segment re-map 0xA0/0xA1 normal/reverse
                        0xA4,                                   // [07] set entire display normal/force on 0xA4/0xA5
                        0xA6,                                   // [08] set normal/reverse display 0xA6/0xA7
                        0xA8, 0x3F,                             // [09] set multiplex ratio 0x00-0x3F
                        0xAD, 0x8B,                             // [10] set dc-dc off/on 0x8A-0x8B disable/on when display on
                        (uint8_t)(_rotation ? 0xC8 : 0xC0),     // [13] set output scan direction 0xC0/0xC8
                        0xD3, 0x00,                             // [14] set display offset 0x00-0x3F
                        0xD5, 0x80,                             // [15] set display clock divide ratio/oscillator
                        0xD9, 0x22,                             // [16] set discharge/precharge period 0x00-0xFF
                        0xDA, 0x12,                             // [17] set common pads hardware configuration 0x02/0x12 sequential/alternative
                        0xDB, 0x20,                             // [18] set vcom deselect level 0x00-0xFF
                        0xAF                                    // [11] display off/on 0xAE/0xAF 
                    };
                    
                    _devI2c->WriteRegU8(0x00, init, sizeof(init));

                    sleep_for(milliseconds(100));

					_initialized = true;
				}
				else
				{
                    uint8_t pon[] = {
                        0x00, // is command
                        0xAF  // [11] display off/on 0xAE/0xAF
                    };
					
                    _devI2c->WriteRegU8(0x00, pon, sizeof(pon));
				}

				_isOn = true;
			}     

            Clear();
        }
        else
        {
            delete _devI2c;
            _devI2c = NULL;
        }
    }
}

// 0.02 mA
void DisplaySh1106::PowerOff()
{
    if (_devI2c != NULL)
    {
        if (_isOn)
        {
            Clear();

            uint8_t poff[] = {
                            0x00, // is command
                            0xAE  // [11] display off/on 0xAE/0xAF
            };

            _devI2c->WriteRegU8(0x00, poff, sizeof(poff));

            _devI2c->Close();
            delete _devI2c;
            _devI2c = NULL;
        }
    }
}

bool DisplaySh1106::IsOn()
{
    return _isOn;
}

void DisplaySh1106::SetBrightness(uint8_t brightness)
{
    _brightness = brightness;

    uint8_t cont[] = { 
        0x81, brightness // [05] set contrast control register 0x00-0xFF
    };
    
    _devI2c->WriteRegU8(0x00, cont, sizeof(cont));
}

void DisplaySh1106::SetRotation(bool rotation)
{
    _rotation = rotation;

    uint8_t rot[] = {
        (uint8_t)(_rotation ? 0xA1 : 0xA0),     // [06] set segment re-map 0xA0/0xA1 normal/reverse
        (uint8_t)(_rotation ? 0xC8 : 0xC0),     // [13] set output scan direction 0xC0/0xC8        
    };

    _devI2c->WriteRegU8(0x00, rot, sizeof(rot));
}



void DisplaySh1106::Update()
{
    memcpy(_gray, skBitmap.getPixels(), sizeof(_gray));   

    int k = 0;
    int l = 0;
    int o = (SH_WIDTH - PX_WIDTH) / 2;
    for (int i = 0; i < PX_WIDTH; i++)
    {
        for (int j = 0; j < PX_HEIGHT; j += 8)
        {
            l = 1;
            uint8_t data = 0;

            for (int k = 0; k < 8; k++)
            {
                uint8_t g = _gray[(j + k) * PX_WIDTH + i];                
                data |= (uint8_t)(g > 0 ? l : 0);
                l *= 2;
            }            
            _blwt[j / 8 * SH_WIDTH + i + o] = data;
        }
    }

    for (int i = 0; i < SH_PAGES; i++)
    {        
        _devI2c->WriteRegU8(0x00, 0xB0 + i); // command, page number SH1106 [12]
        _devI2c->WriteRegU8(0x40, &_blwt[i * SH_WIDTH], SH_WIDTH); // data, page data
    }
}