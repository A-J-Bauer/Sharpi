#include "SensorIr28khz.h"

using namespace std;
using namespace chrono;

string SensorIr28khz::description = {
"----------------------------------------------\n"
"Infrared sensor (receiver module), 38khz      \n"
"                                              \n"
"config:                                       \n"
"                                              \n"
"  nothing to do                               \n"
"                                              \n"
"wiring:                                       \n"
"                                              \n"
"       rpi physical pins                      \n"
"                                              \n"
"    3.3V --  1   2                            \n"
"             3   4                            \n"
"             5   6                            \n"
"             7   8          ---------         \n"
"     gnd --  9   10        |  -----  |        \n"
"   (dat) -- 11   12        | |     | |        \n"
"            13   14        |  -----  |        \n"
"            15   16         ---------         \n"
"            17   18          |  |  |          \n"
"            19   20        dat gnd 3.3V       \n"
"            21   22                           \n"
"            23   24                           \n"
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
};

const string SensorIr28khz::CHIP = "/dev/gpiochip0";

const uint32_t SensorIr28khz::NEC_DELTA = 90000;
const uint32_t SensorIr28khz::NEC_BURST = 562500;
const uint32_t SensorIr28khz::NEC_ZERO = 562500;
const uint32_t SensorIr28khz::NEC_ONE = 1687500;
const uint32_t SensorIr28khz::NEC_LEADINGBURST = 562500 * 16; // 9 ms
const uint32_t SensorIr28khz::NEC_LEADINGSPACENEW = 562500 * 8; // 4.5 ms
const uint32_t SensorIr28khz::NEC_LEADINGSPACEREPEAT = 562500 * 4; // 2.25 ms

SensorIr28khz::SensorIr28khz(int pin, bool activeLow, function<void(uint16_t, uint16_t)> callback_nec)
{
	_pin = pin;
	_activeLow = activeLow;
	_callback_nec = callback_nec;	

	stop = false;
}

SensorIr28khz::~SensorIr28khz()
{
	PowerOff();
}

string SensorIr28khz::GetDescription()
{
	return description;
};

const char* SensorIr28khz::GetDescriptionC()
{
	return description.c_str();
}


void SensorIr28khz::PowerOn()
{
	if (!worker.joinable())
	{
		worker = thread(&SensorIr28khz::work, this, _pin, _activeLow);
	}	
}

void SensorIr28khz::PowerOff()
{
	if (worker.joinable())
	{
		stop = true;
		worker.join();
	}	
}


void SensorIr28khz::work(int pin, bool activeLow)
{
	int i, ret;

	struct timespec wt_start = { 0, 100000000 };
	struct timespec wt_code = { 0, 10000000 };
	struct gpiod_line_event evt[NUMSAMPLES];	

	uint32_t nanoseconds = 0;
	uint16_t address = 0;
	uint16_t abit = 1;
	uint16_t command = 0;
	uint16_t cbit = 1;

	bool error = false;

	gpiod_chip* chip = chip = gpiod_chip_open("/dev/gpiochip0");
	if (chip == NULL)
	{
		return;
	}
	
	gpiod_line* line = gpiod_chip_get_line(chip, DevGpio::J8Gpio(pin));
	if (line == NULL)
	{
		return;
	}

	ret = gpiod_line_request_both_edges_events_flags(line, "sharpi", activeLow ? GPIOD_LINE_REQUEST_FLAG_ACTIVE_LOW : 0);
	if (ret == -1)
	{
		return;
	}
	
	ret = 0;

	while (!stop && ret != -1)
	{
		memset(evt, 0, sizeof(evt));		

		error = false;

		i = 0;

		ret = gpiod_line_event_wait(line, &wt_start);
		if (ret != 1) { continue; }

		ret = gpiod_line_event_read(line, &evt[i]);
		if (ret != 0) { continue; }
		
		i++;

		ret = gpiod_line_event_wait(line, &wt_code);
		if (ret != 1) { continue; }

		ret = gpiod_line_event_read(line, &evt[i]);
		if (ret != 0) { continue; }

		nanoseconds = nanodiff(evt[i], evt[i - 1]);

		//   ------------------
		//  |        9 ms      |
		//
		if (evt[i-1].event_type == GPIOD_LINE_EVENT_RISING_EDGE
			&& evt[i].event_type == GPIOD_LINE_EVENT_FALLING_EDGE
			&& equal(NEC_LEADINGBURST, nanoseconds))
		{
			i++;

			ret = gpiod_line_event_wait(line, &wt_code);
			if (ret != 1) { break; }

			ret = gpiod_line_event_read(line, &evt[i]);
			if (ret != 0) { break; }

			nanoseconds = nanodiff(evt[i], evt[i - 1]);

			//   ------------------            
			//  |        9 ms      |  4.5 ms  |
			//                       --------- 
			if (evt[i].event_type == GPIOD_LINE_EVENT_RISING_EDGE && equal(NEC_LEADINGSPACENEW, nanoseconds))
			{
				nanoseconds = 0;
				address = 0;
				abit = 1;
				command = 0;
				cbit = 1;

				ret = 1;

				while (!error && i < 53)
				{
					i++;

					ret = gpiod_line_event_wait(line, &wt_code);
					if (ret != 1) { error = true; break; }

					ret = gpiod_line_event_read(line, &evt[i]);
					if (ret != 0) { error = true; break; }

					nanoseconds = nanodiff(evt[i], evt[i - 1]);
					
					if (evt[i].event_type == GPIOD_LINE_EVENT_FALLING_EDGE && equal(NEC_BURST, nanoseconds))
					{
						i++;

						ret = gpiod_line_event_wait(line, &wt_code);
						if (ret != 1) { error = true; break; }

						ret = gpiod_line_event_read(line, &evt[i]);
						if (ret != 0) { error = true; break; }

						if (evt[i].event_type == GPIOD_LINE_EVENT_RISING_EDGE)
						{
							nanoseconds = nanodiff(evt[i], evt[i - 1]);

							if (equal(NEC_ONE, nanoseconds))
							{
								if (i < 35)
								{
									address |= abit;
								}
								else if (i < 52)
								{
									command |= cbit;
								}
							}
							else if (!equal(NEC_ZERO, nanoseconds))
							{
								error = true; break;
							}

							if (i < 35)
							{
								abit <<= 1;
							}
							else if (i < 52)
							{
								cbit <<= 1;
							}
						}
						else
						{
							error = true;
						}
					}
					else
					{
						error = true;
					}

				}

				_callback_nec(address, command);
			}			
			//   ------------------        
			//  |        9 ms      | 2.25 |
			//                       -----
			else if (equal(NEC_LEADINGSPACEREPEAT, nanoseconds))
			{
				i++;

				ret = gpiod_line_event_wait(line, &wt_code);
				if (ret != 1) { error = true; break; }

				ret = gpiod_line_event_read(line, &evt[i]);
				if (ret != 0) { error = true; break; }

				nanoseconds = nanodiff(evt[i], evt[i - 1]);

				if (evt[i].event_type == GPIOD_LINE_EVENT_FALLING_EDGE && equal(NEC_BURST, nanoseconds))
				{
					_callback_nec(address, command);
				}
			}
		}
	}

	gpiod_line_release(line);

	gpiod_chip_unref(chip);
}