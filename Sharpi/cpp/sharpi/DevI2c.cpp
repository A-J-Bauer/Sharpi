#include "DevI2c.h"


DevI2c::DevI2c(std::string device, uint8_t address)
{    
    _device = device;
    _address = address;
}

DevI2c::~DevI2c() {
    if (_fd > -1)
    {
        Close();
    }
}

bool DevI2c::Open()
{
    _fd = open(_device.c_str(), O_RDWR);

    if (_fd < 0) {
        return false;
    }

    unsigned long funcs;
    if (ioctl(_fd, I2C_FUNCS, &funcs) != 0 || (funcs & I2C_FUNC_I2C) != I2C_FUNC_I2C)
    {
        close(_fd);
        _fd = -1;
        return false;
    }

    return true;
}

bool DevI2c::ReadRegU8(uint8_t reg, uint8_t* data)
{
    return ReadRegU8(reg, data, 1);
}

bool DevI2c::ReadRegU8(uint8_t reg, uint8_t* data, uint16_t length)
{
    bool ret = true;

    struct i2c_msg msgs[2];
    struct i2c_rdwr_ioctl_data rdwr;

    msgs[0].addr = _address;
    msgs[0].flags = 0;
    msgs[0].buf = &reg;
    msgs[0].len = 1;

    msgs[1].addr = _address;
    msgs[1].flags = I2C_M_RD;
    msgs[1].buf = (uint8_t*)malloc(length);
    memset(msgs[1].buf, 0, length);
    msgs[1].len = length;

    rdwr.msgs = msgs;
    rdwr.nmsgs = 2;
   
    ret = ioctl(_fd, I2C_RDWR, &rdwr) == 0;
    
    memcpy(data, msgs[1].buf, length);

    free(msgs[1].buf);

    return ret;
}

bool DevI2c::WriteRegU8(uint8_t reg, uint8_t data)
{
    return WriteRegU8(reg, &data, 1);
}

bool DevI2c::WriteRegU8(uint8_t reg, const uint8_t* data, uint16_t length)
{
    bool ret = true;

    struct i2c_msg msgs[1];
    struct i2c_rdwr_ioctl_data rdwr;

    msgs[0].addr = _address;
    msgs[0].flags = 0;
    msgs[0].buf = (uint8_t*)malloc(length + 1);
    msgs[0].buf[0] = reg;
    memcpy(msgs[0].buf + 1, data, length);
    msgs[0].len = length + 1;

    rdwr.msgs = msgs;
    rdwr.nmsgs = 1;

    ret = ioctl(_fd, I2C_RDWR, &rdwr) == 0;

    free(msgs[0].buf);

    return ret;
}


void DevI2c::Close()
{
    if (_fd > -1)
    {
        close(_fd);
        _fd = -1;
    }
}


