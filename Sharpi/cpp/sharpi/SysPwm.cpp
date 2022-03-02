#include "SysPwm.h"

const path SysPwm::chippath = "/sys/class/pwm/pwmchip0";
const path SysPwm::exppath = "/sys/class/pwm/pwmchip0/export";
const path SysPwm::unexppath = "/sys/class/pwm/pwmchip0/unexport";

const path SysPwm::enblpath[2] = {
	"/sys/class/pwm/pwmchip0/pwm0/enable",
	"/sys/class/pwm/pwmchip0/pwm1/enable"
};
const path SysPwm::perdpath[2] = {
	"/sys/class/pwm/pwmchip0/pwm0/period",
	"/sys/class/pwm/pwmchip0/pwm1/period"
};
const path SysPwm::dutypath[2] = {
	"/sys/class/pwm/pwmchip0/pwm0/duty_cycle",
	"/sys/class/pwm/pwmchip0/pwm1/duty_cycle"
};

ofstream SysPwm::perdstream[2];
ofstream SysPwm::dutystream[2];

bool SysPwm::_created[2] = { false, false };
bool SysPwm::_isOn[2] = { false, false };
bool SysPwm::_waveSet[2] = {false, false};

int SysPwm::_frequency[2] = { 440, 440}; // hz;
int SysPwm::_dutypercent[2] = { 50, 50 }; // %

unsigned long SysPwm::_periodnanoseconds[2];
unsigned long SysPwm::_dutynanoseconds[2];


string SysPwm::description =
"                                                      \n"
"Hardware PWM (pulse width modulation)                 \n"
"                                                      \n"
"config:                                               \n"
"                                                      \n"
"  edit /boot/config.txt                               \n"
"                                                      \n"
"  find 'dtparam=audio=on' and change to 			   \n"
"    #dtparam=audio=on                                 \n"
"                                                      \n"
"  for pwm 0 only add:                                 \n"
"    dtoverlay=pwm,pin=12,func=4                       \n"
"                                                      \n"
"  for pwm 1 only add:                                 \n"
"    dtoverlay=pwm,pin=13,func=4                       \n"
"                                                      \n"
"  for pwm 0 and pwm 1 add:                            \n"
"    dtoverlay=pwm-2chan,pin=12,func=4,pin2=13,func2=4 \n"
"                                                      \n"
"wiring:                                               \n"
"                                                      \n"
"       rpi physical pins     \n"
"                             \n"
"    3.3v -- 1   2  -- 5v     \n"
"            3   4  -- 5v     \n"
"            5   6  -- gnd    \n"
"            7   8            \n"
"    gnd --  9   10           \n"
"           11   12           \n"
"           13   14 -- gnd    \n"
"           15   16           \n"
"           17   18           \n"
"           19   20           \n"
"           21   22           \n"
"           23   24           \n"
"    gnd -- 25   26           \n"
"           27   28           \n"
"           29   30           \n"
"           31   32 -- pwm_0  \n"
"  pwm_1 -- 33   34           \n"
"           35   36           \n"
"           37   38           \n"
"    gnd -- 39   40           \n"
"                             \n";

string SysPwm::GetDescription()
{
	return description;
};


void SysPwm::SetWave(int pwm)
{
	if (!exists(perdpath[pwm]) || !exists(dutypath[pwm]))
	{
		return;
	}
	
	unsigned long dutynanosecondsold = _dutynanoseconds[pwm];

	_periodnanoseconds[pwm] = 1000 * 1000 * 1000 / _frequency[pwm];
	_dutynanoseconds[pwm] = _dutypercent[pwm] * _periodnanoseconds[pwm] / 100;

	if (perdstream[pwm].is_open() && dutystream[pwm].is_open())
	{
		if (dutynanosecondsold < _periodnanoseconds[pwm])
		{
			perdstream[pwm] << _periodnanoseconds[pwm];
			dutystream[pwm] << _dutynanoseconds[pwm];

			perdstream[pwm].flush();
			dutystream[pwm].flush();
		}
		else
		{
			dutystream[pwm] << _dutynanoseconds[pwm];
			perdstream[pwm] << _periodnanoseconds[pwm];

			dutystream[pwm].flush();
			perdstream[pwm].flush();			
		}
		
		_waveSet[pwm] = true;
	}
	
	_waveSet[pwm] = true;
}

void SysPwm::SetFrequency(int pwm, int frequency)
{
	if (pwm != 0 && pwm != 1)
	{
		return;
	}

	if (frequency < 1 || frequency > 60000)
	{
		return;
	}
	
	_frequency[pwm] = frequency;

	SetWave(pwm);
}

void SysPwm::SetDutyCycle(int pwm, int dutypercent)
{
	if (pwm != 0 && pwm != 1)
	{
		return;
	}

	if (dutypercent < 1 || dutypercent > 99)
	{
		return;
	}

	_dutypercent[pwm] = dutypercent;
	
	SetWave(pwm);
}

void SysPwm::SetPeriodNanoSeconds(int pwm, unsigned long nanoseconds)
{
	if (perdstream[pwm].is_open())
	{
		_periodnanoseconds[pwm] = nanoseconds;
		perdstream[pwm] << _periodnanoseconds[pwm];	
		perdstream[pwm].flush();
	}
}

void SysPwm::SetDutyNanoSeconds(int pwm, unsigned long nanoseconds)
{
	if (dutystream[pwm].is_open())
	{
		_dutynanoseconds[pwm] = nanoseconds;		
		dutystream[pwm] << _dutynanoseconds[pwm];
		dutystream[pwm].flush();
	}	
}


void SysPwm::Create(int pwm)
{
	if (pwm != 0 && pwm != 1)
	{
		return;
	}

	if (_created[pwm])
	{
		return;
	}

	if (!exists(exppath))
	{
		return;
	}

	ofstream expstream(exppath);
	if (expstream.is_open())
	{
		expstream << pwm;

		perdstream[pwm].open(perdpath[pwm]);
		dutystream[pwm].open(dutypath[pwm]);

		expstream.close();
	}	

	usleep(100000);
}

void SysPwm::Destroy(int pwm)
{
	if (pwm != 0 && pwm != 1) 
	{
		return;
	}

	if (!_created[pwm])
	{
		return;
	}

	if (!exists(unexppath))
	{
		return;
	}

	if (perdstream[pwm].is_open())
	{
		perdstream[pwm].close();
	}
	
	if (dutystream[pwm].is_open())
	{
		dutystream[pwm].close();
	}	

	ofstream unexpstream(unexppath);
	if (unexpstream.is_open())
	{
		unexpstream << pwm;
	}
	unexpstream.close();

	usleep(100000);
}

void SysPwm::PowerOn(int pwm)
{
	if (pwm != 0 && pwm != 1)
	{
		return;
	}

	if (_isOn[pwm])
	{
		return;
	}

	if (!_waveSet[pwm])
	{
		SetWave(pwm);
	}

	ofstream enblstream(enblpath[pwm]);
	if (enblstream.is_open())
	{
		enblstream << 1;
	}
	enblstream.close();

	_isOn[pwm] = true;
}

void SysPwm::PowerOff(int pwm)
{
	if (pwm != 0 && pwm != 1)
	{
		return;
	}

	if (!_isOn[pwm])
	{
		return;
	}

	if (!exists(enblpath[pwm]))
	{
		return;
	}

	ofstream enblstream(enblpath[pwm]);
	if (enblstream.is_open())
	{
		enblstream << 0;
	}
	enblstream.close();

	_isOn[pwm] = false;
}



