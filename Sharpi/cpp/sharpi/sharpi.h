#define SHARPI __attribute__((visibility("default")))


#include <tuple>
#include <string>

#include "DevGpio.h"
#include "SysPwm.h"
#include "DevSpi.h"
#include "DisplayDrm.h"
#include "DisplayPcd8544.h"
#include "DisplaySh1106.h"
#include "DisplaySsd1351.h"
#include "DisplayTm1637.h"
#include "UsbWorker.h"
#include "Info.h"
#include "AdcMcp3008.h"
#include "SensorAmg8833.h"
#include "SensorIr28khz.h"

#include "include/core/SkImageEncoder.h"
#include "include/core/SkColor.h"
#include "include/core/SkTypeface.h"
#include "include/core/SkFont.h"
#include "include/core/SkFontMgr.h"
#include "include/core/SkBitmap.h"
#include "include/core/SkImage.h"
#include "include/codec/SkCodec.h"


#ifdef __cplusplus
extern "C" {
#endif

	struct rect
	{
		float left;
		float top;
		float right;
		float bottom;
	};

	// SKIA

	typedef struct cskia cskia;

	// font manager

	SHARPI const char* font_get_fonts(void);
	SHARPI cskia* skfont_new_from_file(const char* path, float size, unsigned char edging);
	SHARPI cskia* skfont_new_from_name(const char* name, float size, unsigned char edging);
	SHARPI void skfont_measure_text(cskia* handle, const char* text, unsigned long length, rect& rect);
	SHARPI void skfont_delete(cskia* handle);

	// color

	SHARPI unsigned int skcolor_from_argb(unsigned char a, unsigned char r, unsigned char g, unsigned char b);

	// paint

	SHARPI cskia* skpaint_new();
	SHARPI void skpaint_set_color(cskia* paint, unsigned int color);
	SHARPI unsigned int skpaint_get_color(cskia* paint);
	SHARPI void skpaint_set_stroke(cskia* paint, bool stroke);	
	SHARPI void skpaint_set_stroke_width(cskia* paint, float width);
	SHARPI void skpaint_set_antialias(cskia* paint, bool antialias);
	SHARPI float skpaint_get_stroke_width(cskia* paint);
	SHARPI void skpaint_delete(cskia* handle);

	// bitmap

	SHARPI cskia* skbitmap_new(int width, int height);
	SHARPI cskia* skbitmap_new_from_file(const char* path, int* width, int* height);
	SHARPI bool skbitmap_encode_to_file(const char* path, cskia* bitmap, int format, int quality);
	SHARPI void skbitmap_delete(cskia* bitmap);

	// canvas

	SHARPI cskia* skcanvas_new_from_bitmap(cskia* bitmap);
	SHARPI void skcanvas_delete(cskia* canvas);
	SHARPI void skcanvas_clear(cskia* canvas, unsigned int color);

	SHARPI void skcanvas_draw_text(cskia* canvas, const char* text, unsigned int len, float x, float y, cskia* font, cskia* paint);
	SHARPI void skcanvas_draw_line(cskia* canvas, float x0, float y0, float x1, float y1, cskia* paint);
	SHARPI void skcanvas_draw_bitmap(cskia* canvas, float x, float y, cskia* bitmap);
	SHARPI void skcanvas_draw_bitmap_rect(cskia* canvas, rect& src, rect& dst, cskia* bitmap);
	SHARPI void skcanvas_draw_bitmap_rect_x(cskia* canvas, rect& src, rect& dst, cskia* bitmap, int cubic);

	// gpio
	
	SHARPI const char* devgpio_get_pinout(void);
	
	SHARPI bool devgpio_set_pinmode(int pin, int mode);
	SHARPI int devgpio_get_pinmode(int pin);

	SHARPI void devgpio_digitalwrite(int pin, bool value);
	SHARPI bool devgpio_digitalread(int pin);

	// pwm	

	SHARPI const char* syspwm_get_description(void);

	SHARPI void syspwm_set_frequency(int pwm, int frequency);
	SHARPI void syspwm_set_dutycycle(int pwm, int dutypercent);
	SHARPI void syspwm_set_period_nanoseconds(int pwm, unsigned long nanoseconds);
	SHARPI void syspwm_set_duty_nanoseconds(int pwm, unsigned long nanoseconds);
	SHARPI void syspwm_create(int pwm);
	SHARPI void syspwm_destroy(int pwm);
	SHARPI void syspwm_power_on(int pwm);
	SHARPI void syspwm_power_off(int pwm);

	// display common

	typedef struct cdisplay cdisplay;

	SHARPI const char* display_get_description(cdisplay* handle);
	SHARPI bool display_is_on(cdisplay* handle);
	SHARPI void display_power_on(cdisplay* handle);
	SHARPI void display_power_off(cdisplay* handle);	
	SHARPI void display_update(cdisplay* handle);
	SHARPI void display_delete(cdisplay* handle);	

	// display specific

	// drm
	SHARPI cdisplay* display_drm_new(void);
	SHARPI cskia* display_drm_get_bitmap(cdisplay* handle, int* width, int* height);
	/*SHARPI unsigned short display_drm_get_width(cdisplay* handle);
	SHARPI unsigned short display_drm_get_height(cdisplay* handle);*/

	// PCD8544
	SHARPI cdisplay* display_pcd8544_new(void);
	SHARPI cdisplay* display_pcd8544_new_x(const char* spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin);
	SHARPI cskia* display_pcd8544_get_bitmap(cdisplay* handle, int* width, int* height);

	// SH1106
	SHARPI cdisplay* display_sh1106_new();	
	SHARPI cdisplay* display_sh1106_new_x(uint8_t i2cAddress, bool rotation, uint8_t contrast);
	SHARPI cdisplay* display_sh1106_new_x2(uint8_t i2cAddress, bool rotation, uint8_t contrast, string i2cDevice);
	SHARPI cskia* display_sh1106_get_bitmap(cdisplay* handle, int* width, int* height);
	SHARPI void display_sh1106_set_brightness(cdisplay* handle, uint8_t brightness);
	SHARPI void display_sh1106_set_rotation(cdisplay* handle, bool rotation);

	// SSD1351
	SHARPI cdisplay* display_ssd1351_new(void);
	SHARPI cdisplay* display_ssd1351_new_x(const char* spiDevice, unsigned char rotation);
	SHARPI cdisplay* display_ssd1351_new_x2(const char* spiDevice, unsigned char rotation, int spiSpeedHz, int dataPin, int resetPin);
	SHARPI cskia* display_ssd1351_get_bitmap(cdisplay* handle, int* width, int* height);

	// TM1637
	SHARPI cdisplay* display_tm1637_new(void);
	SHARPI cdisplay* display_tm1637_new_x(int powerPin, int dataPin, int clockPin);
	SHARPI void display_tm1637_set_brightness(cdisplay* handle, int zerotofour);
	SHARPI void display_tm1637_set_rotation(cdisplay* handle, bool rotate);
	SHARPI void display_tm1637_set_text(cdisplay* handle, const char * text);



	// usb worker

	typedef struct cusbworker cusbworker;

	SHARPI cusbworker* usb_worker_new(int deviceId, int baud, int deadAfterMs, void(*callback_data)(char*), void(*callback_state)(int));
	SHARPI void usb_worker_write(cusbworker* handle, const char* data);
	SHARPI void usb_worker_delete(cusbworker* usbworker);

	// info

	struct meminfo
	{
		unsigned long total;
		unsigned long vmsize;
		unsigned long rsize;
		unsigned long rshrd;
		unsigned long rprvt;		
	};

	SHARPI void info_get_memory(meminfo& mem);
	
	// adc common

	typedef struct cadc cadc;

	SHARPI const char* adc_get_description(cadc* handle);
	SHARPI void adc_delete(cadc* handle);

	// adc specific

	// MCP3008
	SHARPI cadc* adc_mcp3008_new(void);
	SHARPI cadc* adc_mcp3008_new_x(const char* spiDevice);
	SHARPI cadc* adc_mcp3008_new_x2(const char* spiDevice, int spiSpeedHz);	
	SHARPI int adc_mcp3008_read(cadc* handle, int channel);


	// sensor common

	typedef struct csensor csensor;

	SHARPI const char* sensor_get_description(csensor* handle);
	SHARPI void sensor_delete(csensor* handle);

	// sensor specific

	// AMG8833
	SHARPI csensor* sensor_amg8833_new(uint8_t i2cAddress);
	SHARPI csensor* sensor_amg8833_new_x(uint8_t i2cAddress, string i2cDevice);

	SHARPI void sensor_amg8833_power_on(csensor* handle);
	SHARPI void sensor_amg8833_power_off(csensor* handle);

	SHARPI void sensor_amg8833_clear_status(csensor* handle, bool overflow, bool interrupt);
	SHARPI void sensor_amg8833_get_status(csensor* handle, bool* overflow, bool* interrupt);

	SHARPI void sensor_amg8833_set_moving_average_emode(csensor* handle, bool on);
	SHARPI void sensor_amg8833_set_frame_rate(csensor* handle, bool high);

	SHARPI void sensor_amg8833_set_interrupt(csensor* handle, uint8_t mode);
	SHARPI void sensor_amg8833_set_interrupt_x(csensor* handle, uint8_t mode, float highTemp, float lowTemp);
	SHARPI void sensor_amg8833_set_interrupt_x2(csensor* handle, uint8_t mode, float highTemp, float lowTemp, float hysteresis);

	SHARPI void sensor_amg8833_readInterrupts(csensor* handle, uint8_t values[64]);
	
	SHARPI void sensor_amg8833_read_thermistor_short(csensor* handle, short* value);
	SHARPI void sensor_amg8833_read_thermistor_float(csensor* handle, float* value);

	SHARPI void sensor_amg8833_read_temperatures_short(csensor* handle, short values[64]);
	SHARPI void sensor_amg8833_read_temperatures_float(csensor* handle, float values[64]);
	SHARPI void sensor_amg8833_update_temperatures_bitmap(csensor* handle, float minTemp, float maxTemp);
	SHARPI cskia* sensor_amg8833_get_bitmap(csensor* handle, int* width, int* height);
	
	SHARPI csensor* sensor_ir28khz_new(int pin, bool activeLow, void(*callback_nec)(uint16_t, uint16_t));
	SHARPI void sensor_ir28khz_power_on(csensor* handle);
	SHARPI void sensor_ir28khz_power_off(csensor* handle);

#ifdef __cplusplus
}
#endif

