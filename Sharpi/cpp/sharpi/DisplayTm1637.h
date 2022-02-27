#include <string>

#include "Display.h"

#include "DevGpio.h"

using namespace std;

/* Differences between and I2C protocol and the custom protocol designed by Titan Micro Electroncs, the manufacturer. 
TM explicitly says their protocol (call it TMP) is not compatible with I2C, TM pared the protocol down to only what they needed.

These are the major differences:

I2C has addresses so a master can drive multiple slaves. TMP doesn't have this. So I2C is a bus and TMP is point-to-point. This means each display needs two lines. No daisy-chaining either. This also means TM didn't have to pay to get a slave address assigned to their chip. BTW I2C assigns the lowest bit in the address byte to indicate the direction of the data flow. Alternatively you may regard the slave as having two consecutive addresses.
I2C specifies that after the address comes the register address within the slave. TMP is flatter, you send a command followed by any data expected.
I2C sends the most significant bit first, TMP sends the least significant bit first. This only shows up in the low-level bit-banging.
I2C has the concept of clock-stretching. TMP has no way for the slave to tell the master to wait. It isn't needed as the chip will respond within the specified time. There is also no need to worry about a pull-up resistor as a result.
I2C specifies various clock speed tiers. TMP only implements their speed, a max of about 250kHz, which is fast enough.
As it's a custom protocol you won't be able to use any silicon or firmware assists for I2C that may be present.*/




class DisplayTm1637 : Display {

private:
    const uint8_t SH_A = 0;
    const uint8_t SH_B = 1;
    const uint8_t SH_C = 2;
    const uint8_t SH_D = 3;
    const uint8_t SH_E = 4;
    const uint8_t SH_F = 5;
    const uint8_t SH_G = 6;
    const uint8_t SH_DP = 7;

//      A                D    
//     ---              ---
//  F |   | B        C |   | E
//     -G-      ->      -G-
//  E |   | C        B |   | F
//     ---              ---
//      D                A
    
    const uint8_t shrot_fr[3] = { SH_A, SH_F, SH_E };
    const uint8_t shrot_to[3] = { SH_D, SH_C, SH_B };

    const uint8_t COMM1 = 0x40;
    const uint8_t COMM2 = 0xC0;
    const uint8_t COMM3 = 0x80;

    static const uint8_t digitToSegment[];
    const int UNUSED_PIN = -1;
    const int UNDEFINED_PIN = -2;
    const int BITDELEAY = 10;
    const int POWERPIN = 15;
    const int DATAPIN = 16;
    const int CLOCKPIN = 18;
    const uint8_t BRIGHTNESS = 0b00001010;
   
    int _bitDelay = BITDELEAY;
    int _powerPin = UNDEFINED_PIN;
    int _dataPin = UNDEFINED_PIN;
    int _clockPin = UNDEFINED_PIN;
    
    uint8_t _brightness = BRIGHTNESS;
    bool _rotate = false;
    uint8_t _brightnessSaved = BRIGHTNESS;
    uint8_t data[4] = { 0, 0, 0, 0 };
    uint8_t datar[4] = { 0,0,0,0 };

    static map<char16_t, uint8_t> CharToData;

    bool _isOn = false;

    static string description;

public:
    DisplayTm1637();
    DisplayTm1637(int powerPin, int dataPin, int clockPin);
    ~DisplayTm1637();

    string GetDescription();
    const char* GetDescriptionC();

    void PowerOn();
    void PowerOff();
    bool IsOn();

    void Update();
       
    void SetBrightness(int zerotofour);
    void SetRotation(bool rotate);
    void SetText(string text);
    
private:
    void SetSegments(const uint8_t segments[], uint8_t pos = 0, uint8_t length = 4);
    void StartWrite();
    void StopWrite();
    bool WriteByte(uint8_t b);    

};