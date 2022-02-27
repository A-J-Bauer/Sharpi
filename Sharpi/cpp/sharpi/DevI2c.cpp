#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include <include/linux/i2c-dev.h>

#include "DevI2c.h"

DevI2c::DevI2c(string device, uint8_t i2caddress)
{
    _device = device;
    _i2caddress = i2caddress;
}

DevI2c::~DevI2c() {
    if (_fd > -1)
    {
        Close();
    }
}

//bool DevI2c::Open()
//{
//    _fd = open(_device.c_str(), O_RDWR);
//
//    if (_fd < 0) {
//        return false;
//    }
//
//    if (ioctl(file, I2C_SLAVE, _i2caddress) < 0) {
//        return false;
//    }
//
//    return true;
//}
//
//int DevI2c::Read(uint8_t* rx_buffer, uint32_t rx_len)
//{
//    if (read(_fd, buf, 1) != 1) {
//        /* ERROR HANDLING: i2c transaction failed */
//    }
//    else {
//        /* buf[0] contains the read byte */
//    }
//
//    return ioctl(_fd, SPI_IOC_MESSAGE(1), spi_message);
//}
//
//int DevI2c::Write(uint8_t* tx_buffer, uint32_t tx_len)
//{
//    struct spi_ioc_transfer spi_message[1];
//    memset(spi_message, 0, sizeof(spi_message));
//
//    spi_message[0].tx_buf = (unsigned long)tx_buffer;
//    spi_message[0].len = tx_len;
//
//    return ioctl(_fd, SPI_IOC_MESSAGE(1), spi_message);
//}
//
//void DevI2c::Close()
//{
//    if (_fd > -1)
//    {
//        close(_fd);
//        _fd = -1;
//    }
//}
//
//
//void DevI2c::Release()
//{
//    delete this;
//}
