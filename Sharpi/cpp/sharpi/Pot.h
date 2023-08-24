#pragma once

#include <string>

/// <summary>
/// The base class for pots, all pots participating in the C-Api have to derive from this
/// </summary>
class Pot
{

public:
    Pot() {};
    virtual ~Pot() {};
    virtual std::string GetDescription() = 0;
    virtual const char* GetDescriptionC() = 0;
    virtual void PowerOn() = 0;
    virtual void PowerOff() = 0;
};


