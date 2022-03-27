#include "SensorAmg8833.h"

string SensorAmg8833::description =
"----------------------------------------------\n"
"Amg8833 also known as Grid-Eye                \n"
"                                              \n"
"config:                                       \n"
"                                              \n"
"  edit /boot/config.txt                       \n"
"  dtparam=i2c_arm=on,i2c_arm_baudrate=400000  \n"
"                                              \n"
"wiring:                                       \n"
"                                              \n"
"       rpi physical pins                      \n"
"                                              \n"
"     vin --  1   2                            \n"
"     sda --  3   4                            \n"
"     scl --  5   6                            \n"
"             7   8                            \n"
"     gnd --  9   10            -------------  \n"
"   (int) -- 11   12      vin -|             | \n"
"            13   14       3v -|     ---     | \n"
"            15   16      gnd -|    |   |    | \n"
"            17   18      sda -|    | # |    | \n"
"            19   20      scl -|     ---     | \n"
"            21   22      int -|             | \n"
"            23   24            -------------  \n"
"            25   26                           \n"
"            27   28                           \n"
"            29   30                           \n"
"            31   32                           \n"
"            33   34                           \n"
"            35   36                           \n"
"            37   38                           \n"
"            39   40                           \n"
"                                              \n"
"----------------------------------------------\n"
;

const string SensorAmg8833::I2CDEVICE = "/dev/i2c-1";

SensorAmg8833::SensorAmg8833(uint8_t i2cAddress)
	: SensorAmg8833::SensorAmg8833(i2cAddress, I2CDEVICE)
{
}

SensorAmg8833::SensorAmg8833(uint8_t i2cAddress, string i2cDevice)
{
	_i2cAddress = i2cAddress;
	_i2cDevice = i2cDevice;

	_skImageInfo = SkImageInfo::Make(8, 8, SkColorType::kBGRA_8888_SkColorType, SkAlphaType::kPremul_SkAlphaType);
	skBitmap.allocPixels(_skImageInfo);	
}

SensorAmg8833::~SensorAmg8833()
{
	PowerOff();
}

string SensorAmg8833::GetDescription()
{
	return description;
};

const char* SensorAmg8833::GetDescriptionC()
{
	return description.c_str();
}

void SensorAmg8833::PowerOn()
{	
	if (_devI2c == NULL)
	{
		_devI2c = new DevI2c(_i2cDevice, _i2cAddress);
		if (_devI2c->Open())
		{
			sleep_for(milliseconds(100));
			_devI2c->WriteRegU8(Regs::PCLT_RW::addr, Regs::PCLT_RW::OperatingMode::normal);
			sleep_for(milliseconds(50));
			_devI2c->WriteRegU8(Regs::RST_W::addr, Regs::RST_W::ResetType::initial);
			sleep_for(milliseconds(2));
			_devI2c->WriteRegU8(Regs::RST_W::addr, Regs::RST_W::ResetType::flag);
			sleep_for(milliseconds(300));
		}
		else
		{
			delete _devI2c;
			_devI2c = NULL;
		}
	}
}

void SensorAmg8833::PowerOff()
{
	if (_devI2c != NULL)
	{
		_devI2c->WriteRegU8(Regs::PCLT_RW::addr, Regs::PCLT_RW::OperatingMode::sleep);

		_devI2c->Close();
		delete _devI2c;
		_devI2c = NULL;
	}
}

void SensorAmg8833::ClearStatus(bool overflow, bool interrupt)
{
	if (_devI2c == NULL) { return; };

	uint8_t status = 0x00;
	status |= overflow ? Regs::SCLR_W::ClearBits::overflow : 0x00;
	status |= interrupt ? Regs::SCLR_W::ClearBits::interrupt : 0x00;

	_devI2c->WriteRegU8(Regs::SCLR_W::addr, status);
}

void SensorAmg8833::GetStatus(bool* overflow, bool* interrupt)
{
	if (_devI2c == NULL) { return; };

	uint8_t status;
	_devI2c->ReadRegU8(Regs::STAT_R::addr, &status);

	*overflow = status & 0b100;
	*interrupt = status & 0b10;
}

void SensorAmg8833::SetMovingAverageMode(bool on)
{
	if (_devI2c == NULL) { return; };

	_devI2c->WriteRegU8(Regs::AVE_RW::addr, on ? Regs::AVE_RW::MovingAverageMode::on : Regs::AVE_RW::MovingAverageMode::off);
}

void SensorAmg8833::SetFrameRate(bool high)
{
	if (_devI2c == NULL) { return; };

	_devI2c->WriteRegU8(Regs::FPSC_RW::addr, high ? Regs::FPSC_RW::FramesPerSecond::fps10 : Regs::FPSC_RW::FramesPerSecond::fps1);
}

void SensorAmg8833::SetInterrupt(InterruptMode interruptMode)
{	
	if (_devI2c == NULL) { return; };

	switch (interruptMode)
	{
	case SensorAmg8833::None:
		_devI2c->WriteRegU8(Regs::INTC_RW::addr, Regs::INTC_RW::InterruptMode::difference | Regs::INTC_RW::InterruptEnable::inactive);
		break;
	case SensorAmg8833::Absolute:
		_devI2c->WriteRegU8(Regs::INTC_RW::addr, Regs::INTC_RW::InterruptMode::absolute | Regs::INTC_RW::InterruptEnable::active);
		break;
	case SensorAmg8833::Difference:
		_devI2c->WriteRegU8(Regs::INTC_RW::addr, Regs::INTC_RW::InterruptMode::difference | Regs::INTC_RW::InterruptEnable::active);
		break;
	default:
		break;
	}
}

void SensorAmg8833::SetInterrupt(InterruptMode interruptMode, float highTemp, float lowTemp)
{
	if (_devI2c == NULL) { return; };

	if (highTemp > lowTemp)
	{
		SetInterrupt(interruptMode, highTemp, lowTemp, 0);
	}
}

void SensorAmg8833::SetInterrupt(InterruptMode interruptMode, float highTemp, float lowTemp, float hysteresis)
{
	if (_devI2c == NULL) { return; };

	if (highTemp > lowTemp && hysteresis <= (highTemp - lowTemp))
	{
		short high = ConvFloatToShort(highTemp);
		uint8_t highRv[2];
		ConvTemperatureShortToReg(highRv, high);

		short low = ConvFloatToShort(lowTemp);
		uint8_t lowRv[2];
		ConvTemperatureShortToReg(lowRv, low);

		short hyst = ConvFloatToShort(hysteresis);
		uint8_t hystRv[2];
		ConvTemperatureShortToReg(hystRv, hyst);

		_devI2c->WriteRegU8(Regs::INTHL_RW::addr, highRv[0]);
		_devI2c->WriteRegU8(Regs::INTHH_RW::addr, highRv[1]);

		_devI2c->WriteRegU8(Regs::INTLL_RW::addr, lowRv[0]);
		_devI2c->WriteRegU8(Regs::INTLH_RW::addr, lowRv[1]);

		_devI2c->WriteRegU8(Regs::IHYSL_RW::addr, hystRv[0]);
		_devI2c->WriteRegU8(Regs::IHYSH_RW::addr, hystRv[1]);

		SetInterrupt(interruptMode);
	}
}

void SensorAmg8833::ReadInterrupts(uint8_t values[8][8])
{
	memset(values, 0, 64);

	if (_devI2c == NULL) { return; };

	uint8_t itrpts[8];

	_devI2c->ReadRegU8(Regs::INT_R::addr, itrpts, Regs::INT_R::size);

	for (int i = 0; i < 8; i++)
	{
		uint8_t m = 0b00000001;

		for (int j = 0; j < 8; j++)
		{
			values[i][j] = ((itrpts[i] & m) == m ? 1 : 0);
			m <<=1;
		}		
	}
}


void SensorAmg8833::ReadThermistorShort(short* value)
{			
	if (_devI2c == NULL) { return; };

	uint8_t ths[Regs::TTH_R::size];

	_devI2c->ReadRegU8(Regs::TTH_R::addr, ths, Regs::TTH_R::size);
	ConvThermistorRegToShort(value, ths);
}

void SensorAmg8833::ReadThermistorFloat(float* value)
{	
	if (_devI2c == NULL) { return; };

	short sval;
	ReadThermistorShort(&sval);
	*value = ConvShortToFloat(sval);
}

void SensorAmg8833::ReadTemperaturesShort(short values[8][8])
{
	if (_devI2c == NULL) { return; };

	uint8_t tos[2];
	uint8_t to[Regs::TO_R::size];

	_devI2c->ReadRegU8(Regs::TO_R::addr, to, Regs::TO_R::size);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			memcpy(tos, &to[(i * 8 + j) * 2], 2);
			ConvTemperatureRegToShort(&values[i][j], tos);			 
		}
	}
}

void SensorAmg8833::ReadTemperaturesFloat(float values[8][8])
{
	if (_devI2c == NULL) { return; };

	short svals[8][8];
	ReadTemperaturesShort(svals);
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{			
			values[i][j] = ConvShortToFloat(svals[i][j]);
		}
	}
}

void SensorAmg8833::UpdateTemperaturesBitmap(float minTemp, float maxTemp)
{
	if (minTemp > maxTemp) { return; };

	float values[8][8];
	ReadTemperaturesFloat(values);
	
	SkScalar hsv[3] = {0, 1, 0.7}; // hue in [0,..,360)	, saturation and value in [0,..,1]

	float temp;
	float range = maxTemp - minTemp;

	SkColor color;


	SkColor* pixel = (SkColor *)(skBitmap.getPixels());

	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{			
			temp = max(min(values[i][j], maxTemp), minTemp);

			hsv[0] = 256 *((minTemp - temp) / range + 1);

			*pixel = SkHSVToColor(hsv);
			pixel++;			
		}
	}	
}

void SensorAmg8833::ConvThermistorRegToShort(short* val, uint8_t regVal[2])
{	
	*val = ((short)(regVal[1] & 0x07) << 8) | regVal[0];

	if ((regVal[1] & 0x08) != 0)
	{
		*val *= -1;
	}

	*val *= 16;
}

void SensorAmg8833::ConvTemperatureRegToShort(short* val, uint8_t regVal[2])
{
	*val = ((short)(regVal[1] & 0x07) << 8) | regVal[0];

	if ((regVal[1] & 0x08) != 0)
	{
		*val -= 2048;
	}

	*val *= 64;	
}

void SensorAmg8833::ConvTemperatureShortToReg(uint8_t regVal[2], short val)
{
	val /= 64;
	regVal[0] = val & 0xFF;
	regVal[1] = val & 0x600 | (val < 0 ? 0x800 : 0);
}

short SensorAmg8833::ConvFloatToShort(float value)
{
	return((value > 0) ? (short)(value * 256.0f + 0.5f) : (short)(value * 256.0f - 0.5f));
}

float SensorAmg8833::ConvShortToFloat(short value)
{
	return((float)value / 256.0f);
}
