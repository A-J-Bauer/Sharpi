#include <unistd.h>

#include <chrono>
#include <thread>
#include <string>

#include "Display.h"
#include "DevI2c.h"
#include "DevGpio.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"

using namespace std;
using namespace std::this_thread;
using namespace chrono;

class DisplaySh1106 : Display
{
private:
    static string description;

private:
    static const string I2CDEVICE;
    
    static const uint8_t SH_WIDTH = 132; // SH1106 SRAM 132x64
    static const uint8_t SH_HEIGHT = 64;
    static const uint8_t SH_PAGES = SH_HEIGHT / 8;
    static const uint8_t PX_WIDTH = 128;
    static const uint8_t PX_HEIGHT = 64;

    uint8_t _i2cAddress;
    unsigned char _rotation = 0;
    unsigned char _brightness = 128;
    string _i2cDevice = I2CDEVICE;

    bool _isOn = false;
    bool _initialized = false;

    DevI2c* _devI2c = NULL;

    unsigned char _gray[PX_WIDTH * PX_HEIGHT];
    unsigned char _blwt[SH_PAGES * SH_WIDTH];

    SkImageInfo _skImageInfo;

private:
    void Clear();

public:
    SkBitmap skBitmap;

    DisplaySh1106(uint8_t i2cAddress = 0x3C, bool rotate = false, uint8_t brightness = 128);
    DisplaySh1106(uint8_t i2cAddress, bool rotate, uint8_t brightness, string i2cDevice);
    ~DisplaySh1106();

    string GetDescription();
    const char* GetDescriptionC();
    
    void PowerOn();
    void PowerOff();
    bool IsOn();

    void SetBrightness(uint8_t brightness);
    void SetRotation(bool rotation);

    void Update();
};
