#include "AdcMcp3008.h"

string AdcMcp3008::description =
"------------------------------------------------------------\n"
"MCP3008 8-Channel 10-Bit A/D Converter                      \n"
"       Microchip Technology Inc.                            \n"
"                                                            \n"
"config:                                                     \n"
"                                                            \n"
"  edit /boot/config.txt                                     \n"
"  dtparam=spi=on                                            \n"
"  dtoverlay=spi1-1cs (optional for SPI 2)                   \n"
"                                                            \n"
"wiring:                                                     \n"
"                                                            \n"
"       rpi physical pins                                    \n"
"                                                            \n"
"  _0 for SPI 0 and _1 for SPI 1                             \n"
"                                                            \n"
"     3.3v -- 1   2  -- 5v                                   \n"
"             3   4  -- 5v                                   \n"
"             5   6  -- gnd                                  \n"
"             7   8                                          \n"
"     gnd --  9   10                       --__--            \n"
"            11   12 -- cs_1       ch0 -- | o    | -- vdd    \n"
"   rst_1 -- 13   14 -- gnd        ch1 -- |      | -- vref   \n"
"    dc_1 -- 15   16 -- dc_0       ch2 -- |      | -- a_gnd  \n"
"            17   18 -- rst_0      ch3 -- |      | -- clk    \n"
"   din_0 -- 19   20               ch4 -- |      | -- dout   \n"
"  dout_0 -- 21   22               ch5 -- |      | -- din    \n"
"   clk_0 -- 23   24 -- cs_0       ch6 -- |      | -- cs     \n"
"     gnd -- 25   26               ch7 -- |      | -- d_gnd  \n"
"            27   28                       ------            \n"
"            29   30                                         \n"
"            31   32                                         \n"
"            33   34                                         \n"
"            35   36                                         \n"
"            37   38 -- din_1                                \n"
"            39   40 -- clk_1                                \n"
"------------------------------------------------------------\n"
;

string AdcMcp3008::GetDescription()
{
	return description;
};

const char* AdcMcp3008::GetDescriptionC()
{
	return description.c_str();
}

const string AdcMcp3008::SPIDEVICE = "/dev/spidev0.0";
const int AdcMcp3008::SPISPEEDHZ = 10000; // manual page 21 / 6.2

/// <summary>
/// Default constructor
/// spiDevice "/dev/spidev0.0"
/// SPI0 pins see GetDescription
/// </summary>
AdcMcp3008::AdcMcp3008()
	: AdcMcp3008(SPIDEVICE)
{
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="spiDevice">"/dev/spidev0.0" or "/dev/spidev1.0"</param>
AdcMcp3008::AdcMcp3008(string spiDevice)
	: AdcMcp3008(spiDevice, SPISPEEDHZ)
{
}

/// <summary>
/// Constructor
/// </summary>
/// <param name="spiDevice"></param>
/// <param name="spiSpeedHz"></param>
AdcMcp3008::AdcMcp3008(string spiDevice, int spiSpeedHz)
{
	_spiDevice = spiDevice == "/dev/spidev1.0" ? spiDevice : "/dev/spidev0.0";
	unsigned char busidx = _spiDevice == "/dev/spidev1.0" ? 1 : 0;
	
	_spiSpeedHz = spiSpeedHz;

	devSpi = new DevSpi(_spiDevice, 0, 8, _spiSpeedHz, 0);
	devSpi->Open();	
}

AdcMcp3008::~AdcMcp3008()
{	
	devSpi->Close();
	delete devSpi;
}


//                 0                          1
// DIN  |0 |0 |0 |0 |0 |0 |0 | 1|     |SG|D2|D1|D0|- |- |- |- |     |- |- |- |- |- |- |- |- |
// 
// DOUT |- |- |- |- |- |- |- | -|     |- |- |- |- |- |0 |B9|B8|     |B7|B6|B5|B4|B3|B2|B1|B0|

// single ended

// SG D2 D1 D0
// 1  0  0  0   CH0
// 1  0  0  1   CH1
// 1  0  1  0   CH2
// 1  0  1  1   CH3
// 1  1  0  0   CH4
// 1  1  0  1   CH5
// 1  1  1  0   CH6
// 1  1  1  1   CH7

int AdcMcp3008::Read(uint8_t channel)
{
	if (channel < 0 || channel > 7)
	{
		return -1;
	}

	uint8_t tx_buf[3] = { 0x1, (uint8_t)(0b10000000 | channel), 0x0 };
	uint8_t rx_buf[3];

	devSpi->Xfer(tx_buf, rx_buf, 3);

	uint16_t ad = (rx_buf[1] & 0x3) << 8 | rx_buf[2];

	return ad;
}