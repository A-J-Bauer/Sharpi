#include <unistd.h>
#include <string>

#include "Display.h"
#include "DevSpi.h"
#include "DevGpio.h"

#include "include/core/SkCanvas.h"
#include "include/core/SkBitmap.h"

using namespace std;

class DisplayPcd8544 : Display {
private:
    static string description;

private:
    const int UNDEFINED_PIN = -1;
    const int SPISPEEDHZ = 4000000;
    const int POWERPIN = 15;
    const int DATAPIN = 16; // low command, high data
    const int RESETPIN = 18;

    enum PCD8544_CMD : unsigned char
    {
        FunctionSet = 0b00100010, // Vertical addressing !
        DisplayControl = 0x08,
        SetYAddr = 0x40,
        SetXAddr = 0x80,
        SetTemperature = 0x04,
        SetBias = 0x10,
        SetVop = 0x80
    };

    string _spiDevice = "/dev/spidev0.0";
    int _spiSpeedHz = SPISPEEDHZ;
    int _powerPin = UNDEFINED_PIN;
    int _dataPin = UNDEFINED_PIN;
    int _resetPin = UNDEFINED_PIN;

    size_t _bufferSize = 84 * 48 / 8;
    unsigned char _blank[84 * 48 / 8];
    
    bool _isOn = false;

    unsigned char _gray[84 * 48];
    unsigned char _blwt[84 * 48 / 8];
    
    DevSpi* devSpi=NULL;
    SkImageInfo skImageInfo;
    
private:
    void SendCmd(PCD8544_CMD cmd, unsigned char cmdExt);
    void SendData(unsigned char* buffer, unsigned int len);
  
public:    
    SkBitmap skBitmap;

    DisplayPcd8544();
    DisplayPcd8544(string spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin);
    ~DisplayPcd8544();

    string GetDescription();
    const char* GetDescriptionC();

    void PowerOn();
    void PowerOff();
    bool IsOn();

    void Update();

};


