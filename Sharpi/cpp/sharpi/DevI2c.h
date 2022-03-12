#pragma once

#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <string>

#include "include/linux/i2c.h"
#include "include/linux/i2c-dev.h"

//extern "C" {
//    #include "i2c/include/smbus.h"
//}

using namespace std;

class DevI2c {
public:    
    DevI2c(std::string device, uint8_t address);
    ~DevI2c();

    bool Open();    

    bool ReadRegU8(uint8_t reg, uint8_t* data);
    bool ReadRegU8(uint8_t reg, uint8_t* data, uint16_t length);

    bool WriteRegU8(uint8_t reg, uint8_t data);
    bool WriteRegU8(uint8_t reg, const uint8_t* data, uint16_t length);

    void Close();

private:
    int _fd = -1;
    string _device;
    uint8_t _address;
};