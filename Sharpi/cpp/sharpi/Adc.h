#pragma once

#include <string>

/// <summary>
/// The base class for adcs, all adcs participating in the C-Api have to derive from this
/// </summary>
class Adc
{

public:
    Adc() {};
    virtual ~Adc() {};
    virtual std::string GetDescription() = 0;
    virtual const char* GetDescriptionC() = 0;    
};


