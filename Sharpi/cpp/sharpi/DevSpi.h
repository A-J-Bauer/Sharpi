#pragma once

#include <string>

using namespace std;

#define DEVSPI_BUFSIZEDEFAULT 4096
#define DEVSPI_BUFSIZEFILE "/sys/module/spidev/parameters/bufsiz"

class DevSpi {
public:      
    DevSpi(string device, uint8_t mode, uint8_t bits_per_word, uint32_t speed, uint16_t delay);
    ~DevSpi();

    bool Open();
    
    int Read(uint8_t* rx_buffer, uint32_t rx_len);
    int Write(uint8_t* tx_buffer, uint32_t tx_len);
    int Xfer(uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t rx_tx_len); // write then read

    void Close();

private:        
    static uint32_t _bufsize;

private:
    int _fd = -1;
    string _device;    
    uint8_t _mode;
    uint8_t _bits_per_word;
    uint32_t _speed;
    uint16_t _delay;    
};
