#include <unistd.h>
#include <string>

#include "Display.h"
#include "DevSpi.h"
#include "DevGpio.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"

using namespace std;

class DisplaySsd1351 : Display
{
private:
    static string description;

private:
    static const string SPIDEVICE;
    static const int UNDEFINED_PIN;
    static const int SPISPEEDHZ;    
    static const int DATAPIN[2]; // low command, high data
    static const int RESETPIN[2];
    
    enum SSD1351_CMD : unsigned char
    {
        SetColumn = 0x15,
        WriteRam = 0x5C,
        SetRow = 0x75,
        SetRemap = 0xA0,
        SetStartLine = 0xA1,
        SetDisplayOffset = 0xA2,
        SetNormalDisplay = 0xA6,
        SelectFunction = 0xAB,
        SleepIn = 0xAE,
        SleepOut = 0xAF,
        SetPrecharge = 0xB1,
        SetClockDiv = 0xB3,
        SetVSL = 0xB4,
        SetGPIO = 0xB5,
        SetPrecharge2 = 0xB6,
        SetDeselectVoltageLevel = 0xBE,
        SetContrastABC = 0xC1,
        SetContrastMasterCurrent = 0xC7,
        SetMultiplexorRatio = 0xCA,
        SetCommandLocks = 0xFD
    };

    unsigned char remap = 0b01100100; // 565, C->B->A, Solomon SSD1351 manual
    unsigned char startLine;

    unsigned char _rotation = 0;
    string _spiDevice = SPIDEVICE;
    int _spiSpeedHz = SPISPEEDHZ;    
    int _dataPin = DATAPIN[0];
    int _resetPin = RESETPIN[0];

    bool _isOn = false;  
    bool _initialized = false;

    unsigned char _cmddata[128 * 128 * 2];
    unsigned char _565[128 * 128 * 2];

    DevSpi* _devSpi = NULL;
    SkImageInfo _skImageInfo;

private:
    void Send(SSD1351_CMD cmd, unsigned char *data, size_t size);

public:
    SkBitmap skBitmap;

    DisplaySsd1351();
    DisplaySsd1351(string spiDevice, unsigned char rotation);
    DisplaySsd1351(string spiDevice, unsigned char rotation, int spiSpeedHz, int dataPin, int resetPin);
    ~DisplaySsd1351();

    string GetDescription();
    const char* GetDescriptionC();

    void PowerOn();
    void PowerOff();
    bool IsOn();

    void Update();
};


