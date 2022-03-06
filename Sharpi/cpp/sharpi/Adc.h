#pragma once

#include <string>

using namespace std;

/// <summary>
/// The base class for adcs, all adcs participating in the C-Api have to derive from this
/// </summary>
class Adc
{

public:
    Adc() {};
    virtual ~Adc() {};
    virtual string GetDescription() = 0;
    virtual const char* GetDescriptionC() = 0;    
};


