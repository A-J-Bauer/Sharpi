#include "sharpi.h"

using namespace std;

// font

char* fonts_p = NULL;

SHARPI const char* font_get_fonts(void) 
{
	if (fonts_p)
	{
		delete[] fonts_p;
		fonts_p = NULL;
	}

	SkString fonts;
	SkString delimiter("\n");

	sk_sp<SkFontMgr> mgr(SkFontMgr::RefDefault());
	for (int i = 0; i < mgr->countFamilies(); i++) {
		SkString familyName;
		mgr->getFamilyName(i, &familyName);
		fonts.append(familyName);
		fonts.append(delimiter);
	}

	fonts_p = new char[fonts.size() + 1];
	strcpy(fonts_p, fonts.c_str());
	return fonts_p;
}

SHARPI cskia* skfont_new_from_file(const char* fontpath, float size, unsigned char edging)
{
	sk_sp<SkTypeface> skTypeface = SkTypeface::MakeFromFile(fontpath);
	if (skTypeface == nullptr) {
		return nullptr;
	}
	SkFont* font = new SkFont(skTypeface, size);
	font->setEdging((SkFont::Edging)edging); // alias = 0, antialias = 1, subpixelantialias = 2
	return (cskia*)font;
}

SHARPI cskia* skfont_new_from_name(const char* name, float size, unsigned char edging)
{
	sk_sp<SkTypeface> skTypeface = SkTypeface::MakeFromName(name, SkFontStyle::Normal());
	SkFont* font = new SkFont(skTypeface, size);
	font->setEdging((SkFont::Edging)edging); // alias = 0, antialias = 1, subpixelantialias = 2
	return (cskia*)font;
}

SHARPI void skfont_measure_text(cskia* handle, const char* text, unsigned long length, rect& rect)
{
	SkRect skRect;
	((SkFont*)handle)->measureText(text, length, SkTextEncoding::kUTF8, &skRect);
	rect.left = skRect.fLeft;
	rect.top = skRect.fTop;
	rect.right = skRect.fRight;
	rect.bottom = skRect.fBottom;
}


SHARPI void skfont_delete(cskia* handle)
{
	delete ((SkFont*)handle);
}

// SKIA

SHARPI unsigned int skcolor_from_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b)
{	
	return SkColorSetARGB(a, r, g, b);	
}


// paint

SHARPI cskia* skpaint_new()
{	
	return (cskia*)(new SkPaint());
}

SHARPI void skpaint_set_color(cskia* paint, unsigned int color)
{
	((SkPaint*)paint)->setColor(color);
}
	   
SHARPI unsigned int skpaint_get_color(cskia* paint)
{
	return ((SkPaint*)paint)->getColor();
}

SHARPI void skpaint_set_stroke(cskia* paint, bool stroke)
{
	((SkPaint*)paint)->setStroke(stroke);
}

SHARPI void skpaint_set_stroke_width(cskia* paint, float width)
{
	((SkPaint*)paint)->setStrokeWidth(width);
}

SHARPI float skpaint_get_stroke_width(cskia* paint)
{
	return ((SkPaint*)paint)->getStrokeWidth();
}

SHARPI void skpaint_set_antialias(cskia* paint, bool antialias)
{
	((SkPaint*)paint)->setAntiAlias(antialias);
}

SHARPI void skpaint_delete(cskia* handle)
{
	delete ((SkPaint*)handle);
}

// bitmap

SHARPI cskia* skbitmap_new(int width, int height)
{
	SkBitmap* skBitmap = new SkBitmap();
	SkImageInfo skImageInfo = SkImageInfo::MakeN32Premul(width, height);

	if (skBitmap->tryAllocPixels(skImageInfo))
	{
		return (cskia*)skBitmap;
	}

	if (skBitmap != NULL)
	{
		delete skBitmap;
		skBitmap = NULL;
	}

	return (cskia*)skBitmap;
}

SHARPI cskia* skbitmap_new_from_file(const char* path, int* width, int* height)
{	
	SkBitmap* skBitmap = NULL;
	
	unique_ptr<SkStream> skStream = SkStream::MakeFromFile(path);
	if (skStream)
	{		
		unique_ptr<SkCodec> skCodec = SkCodec::MakeFromStream(move(skStream));
		if (skCodec)
		{				
			SkImageInfo skImageInfo = skCodec->getInfo();

			*width = skImageInfo.width();
			*height = skImageInfo.height();

			skBitmap = new SkBitmap();
						
			if (skBitmap->tryAllocPixels(skImageInfo))
			{
				if (skCodec->getPixels(skImageInfo, skBitmap->getPixels(), skBitmap->rowBytes()) == SkCodec::Result::kSuccess)
				{
					return (cskia*)skBitmap;
				}				
			}			
		}
	}

	if (skBitmap != NULL)
	{
		delete skBitmap;
		skBitmap = NULL;
	}
	
	return (cskia*)skBitmap;
}

SHARPI bool skbitmap_encode_to_file(const char* path, cskia* bitmap, int format, int quality)
{	
	SkFILEWStream skFILEWStream(path);
	if (!skFILEWStream.isValid())
	{
		return false;
	}
	
	return SkEncodeImage(&skFILEWStream, *(SkBitmap*)bitmap, (SkEncodedImageFormat)format, quality);
}


SHARPI void skbitmap_delete(cskia* bitmap)
{
	if (bitmap != NULL)
	{
		delete ((SkBitmap *)bitmap);
	}
}


// canvas

SHARPI void skcanvas_clear(cskia* canvas, unsigned int color)
{
	((SkCanvas*)canvas)->clear(color);	
}

SHARPI cskia* skcanvas_new_from_bitmap(cskia* bitmap)
{
	SkCanvas* canvas = new SkCanvas((const SkBitmap &)*bitmap);
	return (cskia * )canvas;
}

SHARPI void skcanvas_delete(cskia* canvas)
{
	delete ((SkCanvas*)canvas);
}

SHARPI void skcanvas_draw_text(cskia* canvas, const char* text, unsigned int len, float x, float y, cskia* font, cskia* paint)
{	
	((SkCanvas*)canvas)->drawSimpleText(text, len, SkTextEncoding::kUTF8, x, y, (SkFont &)*font, (SkPaint &)*paint);
}

SHARPI void skcanvas_draw_line(cskia* canvas, float x0, float y0, float x1, float y1, cskia* paint)
{	
	((SkCanvas*)canvas)->drawLine(x0, y0, x1, y1, (SkPaint &)*paint);
}

SHARPI void skcanvas_draw_bitmap(cskia* canvas, float x, float y, cskia* bitmap)
{	
	if (bitmap != NULL)
	{
		sk_sp<SkImage> skImage = SkImage::MakeFromBitmap(*(SkBitmap*)bitmap);

		((SkCanvas*)canvas)->drawImage(skImage, x, y);
	}	
}

SHARPI void skcanvas_draw_bitmap_rect(cskia* canvas, rect& src, rect& dst, cskia* bitmap)
{
	if (bitmap != NULL)
	{
		sk_sp<SkImage> skImage = SkImage::MakeFromBitmap(*(SkBitmap*)bitmap);		
		SkRect skSrc = SkRect::MakeLTRB(src.left, src.top, src.right, src.bottom);
		SkRect skDst = SkRect::MakeLTRB(dst.left, dst.top, dst.right, dst.bottom);
		((SkCanvas*)canvas)->drawImageRect(skImage, skSrc, skDst, SkSamplingOptions(), nullptr, SkCanvas::kStrict_SrcRectConstraint);
	}
}

SHARPI void skcanvas_draw_bitmap_rect_x(cskia* canvas, rect& src, rect& dst, cskia* bitmap, int cubic)
{
	if (bitmap != NULL)
	{
		sk_sp<SkImage> skImage = SkImage::MakeFromBitmap(*(SkBitmap*)bitmap);
		SkRect skSrc = SkRect::MakeLTRB(src.left, src.top, src.right, src.bottom);
		SkRect skDst = SkRect::MakeLTRB(dst.left, dst.top, dst.right, dst.bottom);
		SkCubicResampler skCubicResampler(cubic == 0 ? SkCubicResampler::CatmullRom() : SkCubicResampler::Mitchell());
		((SkCanvas*)canvas)->drawImageRect(skImage, skSrc, skDst, SkSamplingOptions(skCubicResampler), nullptr, SkCanvas::kStrict_SrcRectConstraint);
	}
}

// gpio

SHARPI const char* devgpio_get_pinout(void)
{						
	return DevGpio::J8PinOutC();
}


SHARPI bool devgpio_set_pinmode(int pin, int mode)
{
	return DevGpio::SetPinMode(pin, mode);
}

SHARPI int devgpio_get_pinmode(int pin)
{
	return DevGpio::GetPinMode(pin);
}

SHARPI void devgpio_digitalwrite(int pin, bool value)
{
	DevGpio::DigitalWrite(pin, value);
}

SHARPI bool devgpio_digitalwrite_sequence(int pin, DevGpio::Seq* seq, int length)
{
	return DevGpio::DigitalWriteSequence(pin, seq, length);
}

SHARPI bool devgpio_digitalread(int pin)
{
	return DevGpio::DigitalRead(pin);
}


// pwm	

SHARPI const char* syspwm_get_description(void)
{
	return SysPwm::description.c_str();
}

SHARPI void syspwm_set_frequency(int pwm, int frequency)
{
	SysPwm::SetFrequency(pwm, frequency);
}

SHARPI void syspwm_set_dutycycle(int pwm, int dutypercent)
{
	SysPwm::SetDutyCycle(pwm, dutypercent);
}

SHARPI void syspwm_set_period_nanoseconds(int pwm, unsigned long nanoseconds)
{
	SysPwm::SetPeriodNanoSeconds(pwm, nanoseconds);
}

SHARPI void syspwm_set_duty_nanoseconds(int pwm, unsigned long nanoseconds)
{
	SysPwm::SetDutyNanoSeconds(pwm, nanoseconds);
}

SHARPI void syspwm_create(int pwm)
{
	SysPwm::Create(pwm);
}

SHARPI void syspwm_destroy(int pwm)
{
	SysPwm::Destroy(pwm);
}

SHARPI void syspwm_power_on(int pwm)
{
	SysPwm::PowerOn(pwm);
}

SHARPI void syspwm_power_off(int pwm)
{
	SysPwm::PowerOff(pwm);
}


// display common


SHARPI const char* display_get_description(cdisplay* handle) {
	return ((Display*)handle)->GetDescriptionC();	
}

SHARPI bool display_is_on(cdisplay* handle)
{
	return(((Display*)handle)->IsOn());
}

SHARPI void display_power_on(cdisplay* handle)
{
	((Display*)handle)->PowerOn();
}

SHARPI void display_power_off(cdisplay* handle)
{
	((Display*)handle)->PowerOff();
}

SHARPI void display_update(cdisplay* handle)
{
	((Display*)handle)->Update();
}


SHARPI void display_delete(cdisplay* handle) {
	delete ((Display*)handle);
}


// display specific

// DRM
SHARPI cdisplay* display_drm_new(void)
{
	return (cdisplay*)new DisplayDrm();
}

SHARPI cskia* display_drm_get_bitmap(cdisplay* handle, int* width, int* height)
{
	SkBitmap* skBitmap = &((DisplayDrm*)handle)->skBitmap;
	*width = skBitmap->width();
	*height = skBitmap->height();
	return (cskia*)(skBitmap);
}

//SHARPI unsigned short display_drm_get_width(cdisplay* handle)
//{
//	return ((DisplayDrm*)handle)->width;
//}
//
//SHARPI unsigned short display_drm_get_height(cdisplay* handle)
//{
//	return ((DisplayDrm*)handle)->height;
//}


// PCD8544
SHARPI cdisplay* display_pcd8544_new()
{
	return (cdisplay*)new DisplayPcd8544;
};

SHARPI cdisplay* display_pcd8544_new_x(const char* spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin)
{
	return (cdisplay*)new DisplayPcd8544(string(spiDevice), spiSpeedHz, powerPin, dataPin, resetPin);
}

SHARPI cskia* display_pcd8544_get_bitmap(cdisplay* handle, int* width, int* height)
{
	SkBitmap* skBitmap = &((DisplayPcd8544*)handle)->skBitmap;
	*width = skBitmap->width();
	*height = skBitmap->height();
	return (cskia*)(skBitmap);
}


// SH1106
SHARPI cdisplay* display_sh1106_new()
{
	return (cdisplay*)new DisplaySh1106;
}

SHARPI cdisplay* display_sh1106_new_x(uint8_t i2cAddress, bool rotation, uint8_t contrast)
{
	return (cdisplay*)new DisplaySh1106(i2cAddress, rotation, contrast);
}

SHARPI cdisplay* display_sh1106_new_x2(uint8_t i2cAddress, bool rotation, uint8_t contrast, string i2cDevice)
{
	return (cdisplay*)new DisplaySh1106(i2cAddress, rotation, contrast, i2cDevice);
}

SHARPI cskia* display_sh1106_get_bitmap(cdisplay* handle, int* width, int* height)
{
	SkBitmap* skBitmap = &((DisplaySh1106*)handle)->skBitmap;
	*width = skBitmap->width();
	*height = skBitmap->height();
	return (cskia*)(skBitmap);
}


SHARPI void display_sh1106_set_brightness(cdisplay* handle, uint8_t brightness)
{
	((DisplaySh1106*)handle)->SetBrightness(brightness);
}

SHARPI void display_sh1106_set_rotation(cdisplay* handle, bool rotate)
{
	((DisplaySh1106*)handle)->SetRotation(rotate);
}


// SSD1351
SHARPI cdisplay* display_ssd1351_new(void)
{
	return (cdisplay*)new DisplaySsd1351;
}

SHARPI cdisplay* display_ssd1351_new_x(const char* spiDevice, unsigned char rotation)
{
	return (cdisplay*)new DisplaySsd1351(string(spiDevice), rotation);
}

SHARPI cdisplay* display_ssd1351_new_x2(const char* spiDevice, unsigned char rotation, int spiSpeedHz, int dataPin, int resetPin)
{
	return (cdisplay*)new DisplaySsd1351(string(spiDevice), rotation, spiSpeedHz, dataPin, resetPin);
}

SHARPI cskia* display_ssd1351_get_bitmap(cdisplay* handle, int* width, int* height)
{
	SkBitmap* skBitmap = &((DisplaySsd1351*)handle)->skBitmap;
	*width = skBitmap->width();
	*height = skBitmap->height();
	return (cskia*)(skBitmap);	
}


// TM1637
SHARPI cdisplay* display_tm1637_new()
{
	return (cdisplay*)new DisplayTm1637;
}

SHARPI cdisplay* display_tm1637_new_x(int powerPin, int dataPin, int clockPin)
{
	return (cdisplay*)new DisplayTm1637(powerPin, dataPin, clockPin);
}

SHARPI void display_tm1637_set_brightness(cdisplay* handle, int zerotofour)
{
	((DisplayTm1637*)handle)->SetBrightness(zerotofour);
}

SHARPI void display_tm1637_set_rotation(cdisplay* handle, bool rotation)
{
	((DisplayTm1637*)handle)->SetRotation(rotation);
}

SHARPI void display_tm1637_set_text(cdisplay* handle, const char* text)
{
	((DisplayTm1637*)handle)->SetText(text);
}




// usb worker

SHARPI cusbworker* usb_worker_new(int deviceId, int baud, int deadAfterMs, void(*callback_data)(char*), void(*callback_state)(int))
{
	return (cusbworker *)(new UsbWorker(deviceId, baud, deadAfterMs, callback_data, callback_state));
}

SHARPI void usb_worker_write(cusbworker* handle, const char* data)
{
	((UsbWorker*)handle)->Send(data);
}

SHARPI void usb_worker_delete(cusbworker* handle)
{
	delete ((UsbWorker*)handle);
}


// info

SHARPI void info_get_memory(meminfo& mem)
{
	return Info::GetMemory(mem.total, mem.vmsize, mem.rsize, mem.rshrd, mem.rprvt);
}


// adc common

SHARPI const char* adc_get_description(cadc* handle)
{
	return ((Adc*)handle)->GetDescriptionC();
}

SHARPI void adc_delete(cadc* handle)
{
	delete ((Adc*)handle);
}

// adc specific

// MCP3008

SHARPI cadc* adc_mcp3008_new(void)
{
	return (cadc*)new AdcMcp3008;
}

SHARPI cadc* adc_mcp3008_new_x(const char* spiDevice)
{
	return (cadc*)new AdcMcp3008(string(spiDevice));
}

SHARPI cadc* adc_mcp3008_new_x2(const char* spiDevice, int spiSpeedHz)
{
	return (cadc*)new AdcMcp3008(string(spiDevice), spiSpeedHz);
}

SHARPI int adc_mcp3008_read(cadc* handle, int channel)
{
	return ((AdcMcp3008*)handle)->Read(channel);
}





// sensor common

typedef struct csensor csensor;

SHARPI const char* sensor_get_description(csensor* handle)
{
	return ((Sensor*)handle)->GetDescriptionC();
}

SHARPI void sensor_delete(csensor* handle)
{
	delete ((Sensor*)handle);
}

// sensor specific

// AMG8833
SHARPI csensor* sensor_amg8833_new(uint8_t i2cAddress)
{
	return (csensor*)new SensorAmg8833(i2cAddress);
}

SHARPI csensor* sensor_amg8833_new_x(uint8_t i2cAddress, string i2cDevice)
{
	return (csensor*)new SensorAmg8833(i2cAddress, i2cDevice);
}

SHARPI void sensor_amg8833_power_on(csensor* handle)
{
	((SensorAmg8833*)handle)->PowerOn();
}

SHARPI void sensor_amg8833_power_off(csensor* handle)
{
	((SensorAmg8833*)handle)->PowerOff();
}

SHARPI void sensor_amg8833_clear_status(csensor* handle, bool overflow, bool interrupt)
{
	((SensorAmg8833*)handle)->ClearStatus(overflow, interrupt);
}

SHARPI void sensor_amg8833_get_status(csensor* handle, bool* overflow, bool* interrupt)
{
	((SensorAmg8833*)handle)->GetStatus(overflow, interrupt);
}

SHARPI void sensor_amg8833_set_moving_average_emode(csensor* handle, bool on)
{
	((SensorAmg8833*)handle)->SetMovingAverageMode(on);
}

SHARPI void sensor_amg8833_set_frame_rate(csensor* handle, bool high)
{
	((SensorAmg8833*)handle)->SetFrameRate(high);
}

SHARPI void sensor_amg8833_set_interrupt(csensor* handle, uint8_t mode)
{
	((SensorAmg8833*)handle)->SetInterrupt((SensorAmg8833::InterruptMode)mode);
}

SHARPI void sensor_amg8833_set_interrupt_x(csensor* handle, uint8_t mode, float highTemp, float lowTemp)
{
	((SensorAmg8833*)handle)->SetInterrupt((SensorAmg8833::InterruptMode)mode, highTemp, lowTemp);
}

SHARPI void sensor_amg8833_set_interrupt_x2(csensor* handle, uint8_t mode, float highTemp, float lowTemp, float hysteresis)
{
	((SensorAmg8833*)handle)->SetInterrupt((SensorAmg8833::InterruptMode)mode, highTemp, lowTemp, hysteresis);
}

SHARPI void sensor_amg8833_readInterrupts(csensor* handle, uint8_t value[64])
{	
	((SensorAmg8833*)handle)->ReadInterrupts((uint8_t(*)[8])value);
}

SHARPI void sensor_amg8833_read_thermistor_short(csensor* handle, short* value)
{
	((SensorAmg8833*)handle)->ReadThermistorShort(value);
}

SHARPI void sensor_amg8833_read_thermistor_float(csensor* handle, float* value)
{
	((SensorAmg8833*)handle)->ReadThermistorFloat(value);
}

SHARPI void sensor_amg8833_read_temperatures_short(csensor* handle, short values[64])
{
	((SensorAmg8833*)handle)->ReadTemperaturesShort((short(*)[8])values);
}

SHARPI void sensor_amg8833_read_temperatures_float(csensor* handle, float values[64])
{
	((SensorAmg8833*)handle)->ReadTemperaturesFloat((float(*)[8])values);
}

SHARPI void sensor_amg8833_update_temperatures_bitmap(csensor* handle, float minTemp, float maxTemp)
{
	((SensorAmg8833*)handle)->UpdateTemperaturesBitmap(minTemp, maxTemp);	
}

SHARPI cskia* sensor_amg8833_get_bitmap(csensor* handle, int* width, int* height)
{
	SkBitmap* skBitmap = &((SensorAmg8833*)handle)->skBitmap;
	*width = skBitmap->width();
	*height = skBitmap->height();
	return (cskia*)(skBitmap);
}


// IR28KHZ

SHARPI csensor* sensor_ir28khz_new(int pin, bool activeLow, void(*callback_nec)(uint16_t, uint16_t))
{
	return (csensor*)new SensorIr28khz(pin, activeLow, callback_nec);	
}

SHARPI void sensor_ir28khz_power_on(csensor* handle)
{
	((SensorIr28khz*)handle)->PowerOn();
}

SHARPI void sensor_ir28khz_power_off(csensor* handle)
{
	((SensorIr28khz*)handle)->PowerOff();
}


// pot common

typedef struct cpot cpot;

SHARPI const char* pot_get_description(cpot* handle)
{
	return ((Pot*)handle)->GetDescriptionC();
}

SHARPI void pot_delete(cpot* handle)
{
	delete ((Pot*)handle);
}

// pot specific


SHARPI cpot* pot_ds3502_new(uint8_t i2cAddress)
{
	return (cpot*)new PotDs3502(i2cAddress);
}

SHARPI cpot* pot_ds3502_new_x(uint8_t i2cAddress, string i2cDevice)
{
	return (cpot*)new PotDs3502(i2cAddress, i2cDevice);
}

SHARPI void pot_ds3502_power_on(cpot* handle)
{
	((PotDs3502*)handle)->PowerOn();
}

SHARPI void pot_ds3502_power_off(cpot* handle)
{
	((PotDs3502*)handle)->PowerOff();
}

SHARPI uint8_t pot_ds3502_get_wiper(cpot* handle)
{
	return ((PotDs3502*)handle)->GetWiper();
}

SHARPI void pot_ds3502_set_wiper(cpot* handle, uint8_t value)
{
	((PotDs3502*)handle)->SetWiper(value);
}

SHARPI void pot_ds3502_set_wiper_persistent(cpot* handle, uint8_t value)
{
	((PotDs3502*)handle)->SetWiperPersistent(value);
}