#pragma once

#include <string>

#include "Adc.h"
#include "DevSpi.h"
#include "DevGpio.h"

using namespace std;

class AdcMcp3008 : Adc
{
private:
    static string description;

private:
    static const string SPIDEVICE;    
    static const int SPISPEEDHZ;

    string _spiDevice = SPIDEVICE;
    int _spiSpeedHz = SPISPEEDHZ;
    
    DevSpi* devSpi = NULL;
    
public:
    AdcMcp3008();
    AdcMcp3008(string spiDevice);
    AdcMcp3008(string spiDevice, int spiSpeedHz);
    ~AdcMcp3008();

    string GetDescription();
    const char* GetDescriptionC();

    int Read(uint8_t channel);
};