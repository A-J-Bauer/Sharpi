#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>

#include "include/linux/spi/spidev.h"

#include "DevSpi.h"

uint32_t DevSpi::_bufsize = 0;

DevSpi::DevSpi(string device, uint8_t mode, uint8_t bits_per_word, uint32_t speed, uint16_t delay)
{
    if (!_bufsize)
    {
        _bufsize = DEVSPI_BUFSIZEDEFAULT;
        _fd = open(DEVSPI_BUFSIZEFILE, O_RDONLY);
        if (_fd > 0)
        {
            char bufstr[24];
            ssize_t length = read(_fd, bufstr, sizeof(bufstr));
            if (length)
            {
                _bufsize = atoi(bufstr);
            }
            close(_fd);
            _fd = -1;
        }
    }   
    
    _device = device;
    _mode = mode;
    _bits_per_word = bits_per_word;
    _speed = speed;
    _delay = delay;    
}

DevSpi::~DevSpi() {
    if (_fd > -1)
    {
        Close();        
    }
}

bool DevSpi::Open()
{
    _fd = open(_device.c_str(), O_RDWR);

    if (_fd < 0) {
        return false;
    }

    /* Set SPI_POL and SPI_PHA */
    if (ioctl(_fd, SPI_IOC_WR_MODE, &_mode) < 0) {
        return false;
    }
    if (ioctl(_fd, SPI_IOC_RD_MODE, &_mode) < 0) {
        return false;
    }

    /* Set bits per word*/
    if (ioctl(_fd, SPI_IOC_WR_BITS_PER_WORD, &_bits_per_word) < 0) {
        return false;
    }
    if (ioctl(_fd, SPI_IOC_RD_BITS_PER_WORD, &_bits_per_word) < 0) {
        return false;
    }

    /* Set SPI speed*/
    if (ioctl(_fd, SPI_IOC_WR_MAX_SPEED_HZ, &_speed) < 0) {
        return false;
    }
    if (ioctl(_fd, SPI_IOC_RD_MAX_SPEED_HZ, &_speed) < 0) {
        return false;
    }
    
    return true;
}

int DevSpi::Read(uint8_t* rx_buffer, uint32_t rx_len)
{
    struct spi_ioc_transfer spi_message[1];
    memset(spi_message, 0, sizeof(spi_message));
    
    int ret;
    uint64_t rx_buffer_offset = (uint64_t)rx_buffer;
    uint32_t remain = rx_len;
    do
    {
        spi_message[0].rx_buf = rx_buffer_offset;

        if (remain > _bufsize)
        {        
            spi_message[0].len = _bufsize;            
            rx_buffer_offset += _bufsize;
            remain -= _bufsize;
        }
        else
        {            
            spi_message[0].len = remain;            
            remain = 0;
        }

        ret = ioctl(_fd, SPI_IOC_MESSAGE(1), spi_message);

    } while (remain > 0 && ret == spi_message[0].len);
    
    return remain == 0 ? 0 : -1;
}

int DevSpi::Write(uint8_t* tx_buffer, uint32_t tx_len)
{
    struct spi_ioc_transfer spi_message[1];
    memset(spi_message, 0, sizeof(spi_message));

    int ret;
    uint64_t tx_buffer_offset = (uint64_t)tx_buffer;
    uint32_t remain = tx_len;

    do
    {
        spi_message[0].tx_buf = tx_buffer_offset;

        if (remain > _bufsize)
        {
            spi_message[0].len = _bufsize;                        
            tx_buffer_offset += _bufsize;
            remain -= _bufsize;
        }
        else
        {
            spi_message[0].len = remain;            
            remain = 0;
        }

        ret = ioctl(_fd, SPI_IOC_MESSAGE(1), spi_message);

    } while (remain > 0 && ret == spi_message[0].len);

    return remain == 0 ? 0 : -1;
}


int DevSpi::Xfer(uint8_t* tx_buffer, uint8_t* rx_buffer, uint32_t rx_tx_len) // write then read
{
    struct spi_ioc_transfer spi_message[1];
    memset(spi_message, 0, sizeof(spi_message));

    int ret;
    uint64_t tx_buffer_offset = (uint64_t)tx_buffer;
    uint64_t rx_buffer_offset = (uint64_t)rx_buffer;
    uint32_t remain = rx_tx_len;

    do
    {
        spi_message[0].tx_buf = tx_buffer_offset;
        spi_message[0].rx_buf = rx_buffer_offset;

        if (remain > _bufsize)
        {
            spi_message[0].len = _bufsize;            
            tx_buffer_offset += _bufsize;
            remain -= _bufsize;
        }
        else
        {
            spi_message[0].len = remain;            
            remain = 0;
        }

        ret = ioctl(_fd, SPI_IOC_MESSAGE(1), spi_message);

    } while (remain > 0 && ret == spi_message[0].len);

    return remain == 0 ? 0 : -1;
}

void DevSpi::Close()
{
    if (_fd > -1)
    {
        close(_fd);
        _fd = -1;
    }    
}

//
//void DevSpi::Release()
//{
//    delete this;
//}
