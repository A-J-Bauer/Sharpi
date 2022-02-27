#pragma once

#include <string>

using namespace std;


class DevI2c {
public:
    DevI2c(string device, uint8_t i2caddress);
    ~DevI2c();

    /*bool Open();

    int Read(uint8_t* rx_buffer, uint32_t rx_len);
    int Write(uint8_t* tx_buffer, uint32_t tx_len);    

    void Close();*/

    //void Release();
private:
    int _fd = -1;
    string _device = "/dev/i2c-1";
    uint8_t _i2caddress;
};