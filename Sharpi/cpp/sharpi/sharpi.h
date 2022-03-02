#define SHARPI __attribute__((visibility("default")))

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
	SHARPI void skbitmap_delete(cskia* bitmap);

	// canvas

	SHARPI cskia* skcanvas_new_from_bitmap(cskia* bitmap);
	SHARPI void skcanvas_delete(cskia* canvas);
	SHARPI void skcanvas_clear(cskia* canvas, unsigned int color);

	SHARPI void skcanvas_draw_text(cskia* canvas, const char* text, unsigned int len, float x, float y, cskia* font, cskia* paint);
	SHARPI void skcanvas_draw_line(cskia* canvas, float x0, float y0, float x1, float y1, cskia* paint);
	SHARPI void skcanvas_draw_bitmap(cskia* canvas, float x, float y, cskia* bitmap);
	SHARPI void skcanvas_draw_bitmap_rect(cskia* canvas, rect& src, rect& dst, cskia* bitmap);

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

	// PCD8544
	SHARPI cdisplay* display_pcd8544_new(void);
	SHARPI cdisplay* display_pcd8544_new_x(const char* spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin);
	SHARPI cskia* display_pcd8544_get_bitmap(cdisplay* handle, int* width, int* height);

	// SSD1351
	SHARPI cdisplay* display_ssd1351_new(void);
	SHARPI cdisplay* display_ssd1351_new_x(const char* spiDevice, unsigned char rotation);
	SHARPI cdisplay* display_ssd1351_new_x2(const char* spiDevice, unsigned char rotation, int spiSpeedHz, int dataPin, int resetPin);
	SHARPI cskia* display_ssd1351_get_bitmap(cdisplay* handle, int* width, int* height);

	// TM1637
	SHARPI cdisplay* display_tm1637_new(void);
	SHARPI cdisplay* display_tm1637_new_x(int powerPin, int dataPin, int clockPin);
	SHARPI void display_tm1637_set_brightness(cdisplay* display, int zerotofour);
	SHARPI void display_tm1637_set_rotation(cdisplay* display, bool rotate);
	SHARPI void display_tm1637_set_text(cdisplay* display, const char * text);

	// drm
	SHARPI cdisplay* display_drm_new(void);	
	SHARPI cskia* display_drm_get_bitmap(cdisplay* handle, int* width, int* height);
	/*SHARPI unsigned short display_drm_get_width(cdisplay* handle);
	SHARPI unsigned short display_drm_get_height(cdisplay* handle);*/


	// usb worker

	typedef struct cusbworker cusbworker;

	SHARPI cusbworker* usb_worker_new(int deviceId, int baud, int deadAfterMs, void(*callback_data)(char*), void(*callback_state)(int));
	SHARPI void usb_worker_write(cusbworker* handle, const char* data);
	SHARPI void usb_worker_delete(cusbworker* usbworker);


#ifdef __cplusplus
}
#endif

