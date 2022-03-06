#pragma once

#include <string>

using namespace std;

/// <summary>
/// The base class for displays, all displays participating in the C-Api have to derive from this
/// </summary>
class Display {

public:    
    Display() {};
    virtual ~Display() {};
    virtual string GetDescription() = 0;
    virtual const char * GetDescriptionC() = 0;
    virtual bool IsOn() = 0;
    virtual void PowerOn() = 0;
    virtual void PowerOff() = 0;
    virtual void Update() = 0;
};


