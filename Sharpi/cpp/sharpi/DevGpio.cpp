#include "DevGpio.h"

using namespace std;


const char* DevGpio::gpiod_version = gpiod_version_string();

bool DevGpio::Open(string devpath)
{
    if (chip != NULL) {
        return false;
    }

    for (int line = 0; line <= 40; line++) {
        lines[line] = NULL;
    }

	chip = gpiod_chip_open(devpath.c_str());   

	return chip != NULL;
}


void DevGpio::Close() {
    if (chip != NULL)
    {
        for (int i = 1; i <= 40; i++) {
            int line = J8Gpio(i);
            if (0 <= line && line <= 40) {
                if (lines[line] != NULL)
                {
                    gpiod_line_release(lines[line]);
                }                
            }
        }
        gpiod_chip_unref(chip);
        chip = NULL;
    }
}


bool DevGpio::Check(int line)
{
    if (line < MIN_PIN || line > MAX_PIN)
    {
        return false;
    }

    if (chip == NULL)
    {
        if (!Open("/dev/gpiochip0"))
        {
            return false;
        }
    }

    return true;
}


bool DevGpio::SetPinMode(int pin, int mode) 
{
    int line = J8Gpio(pin);
    if (!Check(line))
    {
        return false;
    }

    lines[line] = gpiod_chip_get_line(chip, line);

    if (gpiod_line_is_used(lines[line]))
    {
        return false;
    }

    switch (mode) {            
    case DevGpio::INPUT:        
        if (gpiod_line_request_input_flags(lines[line], consumer, GPIOD_LINE_REQUEST_FLAG_BIAS_DISABLED))
        {
            return true;
        }
        break;
    case DevGpio::OUTPUT:        
        return gpiod_line_request_output(lines[line], consumer, 0) == 0;
        break;   
    case DevGpio::INPUT_PULL_UP:
        if (gpiod_line_request_input_flags(lines[line], consumer, GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_UP))
        {
            return true;
        }
        break;
    case DevGpio::INPUT_PULL_DOWN:
        if (gpiod_line_request_input_flags(lines[line], consumer, GPIOD_LINE_REQUEST_FLAG_BIAS_PULL_DOWN))
        {
            return true;
        }
        break;
    default:
        return false;
    }

    return false;
}

int DevGpio::GetPinMode(int pin)
{
    int line = J8Gpio(pin);
    if (!Check(line))
    {
        return false;
    }

    lines[line] = gpiod_chip_get_line(chip, line);

    int direction = gpiod_line_direction(lines[line]);
    if (direction == GPIOD_LINE_DIRECTION_INPUT)
    {
        int bias = gpiod_line_bias(lines[line]);
        switch (bias)
        {
        case GPIOD_LINE_BIAS_PULL_UP:
            return DevGpio::INPUT_PULL_UP;
        case GPIOD_LINE_BIAS_PULL_DOWN:
            return DevGpio::INPUT_PULL_DOWN;
        case GPIOD_LINE_BIAS_DISABLED:
            return DevGpio::INPUT;
        case GPIOD_LINE_BIAS_UNKNOWN:
            return DevGpio::INPUT_UNKNOWN;
        default: // undefined
            return -1;
       
        }
    }
    else if (direction == GPIOD_LINE_DIRECTION_OUTPUT)
    {
        // for now events (falling/risign edge) are not implemented

        return DevGpio::OUTPUT;
    }
    
    return -1;
}

void DevGpio::DigitalWrite(int pin, bool value)
{
    int line = J8Gpio(pin);
    if (!Check(line))
    {
        return;
    }

    gpiod_line_set_value(lines[line], value);
}

thread DevGpio::sequence[40] = {};
atomic<bool> DevGpio::running[40] = {false };

void DevGpio::sequencer(int pin, Seq* delseq, int length)
{   
    SetPinMode(pin, OUTPUT);

    for (int i = 0; i < length; i++)
    {   
        DigitalWrite(pin, delseq[i].value);
        sleep_for(nanoseconds(delseq[i].nano - 50000)); // about 50000ns off -> nanosleep, init/init_task.c in struct task_struct init_task .timer_slack_ns = 50000   
    }
    
    delete[] delseq;
    running[pin] = false;
}

bool DevGpio::DigitalWriteSequence(int pin, Seq* seq, int length)
{
    int line = J8Gpio(pin);
    if (!Check(line))
    {
        return false;
    }

    if (!running[pin])
    {
        running[pin] = true;     

        Seq* delseq = new Seq[length];
        memcpy(delseq, seq, sizeof(Seq) * length);

        sequence[pin] = thread(sequencer, pin, delseq, length);
        sequence[pin].detach();
    }
    else
    {
        return false;
    }

    return true;
}

bool DevGpio::DigitalRead(int pin)
{
    int line = J8Gpio(pin);
    if (!Check(line))
    {
        return false;
    }
    
    return gpiod_line_get_value(lines[line]) == 1;
}

int DevGpio::J8Gpio(int pin)
{	
	if (0 < pin && pin < J8.size())
	{
		return get<0>(J8[pin]);
	}
	return -1;
}

string DevGpio::J8Info(int pin)
{
	if (0 < pin && pin < J8.size())
	{
		return get<1>(J8[pin]);
	}
	return "undef";
}

string DevGpio::J8PinOut()
{
    if (DevGpio::j8PinOut == "")
    {
        string grn = "\x1B[32m";
        string red = "\x1B[31m";
        string nrm = "\x1B[0m";

        string pinout = "J8:\n";
        for (int i = 1; i < J8.size(); i += 2)
        {
            string odd = get<0>(J8[i]) < -1 ? red + get<1>(J8[i]) + nrm : (get<0>(J8[i]) > -1 ? grn + get<1>(J8[i]) + nrm : get<1>(J8[i]));
            string even = get<0>(J8[i + 1]) < -1 ? red + get<1>(J8[i + 1]) + nrm : (get<0>(J8[i + 1]) > 0 ? grn + get<1>(J8[i + 1]) + nrm : get<1>(J8[i + 1]));
            string pad = "";
            for (int j = 0; j < 11 - get<1>(J8[i]).length(); j++)
            {
                pad += " ";
            }

            pinout += pad + odd + (i < 10 ? "  " : " ") + to_string(i) + "   " + to_string(i + 1) + (i + 1 < 10 ? "  " : " ") + even + "\n";
        }

        DevGpio::j8PinOut = pinout;        
    }

    return DevGpio::j8PinOut;
}

const char* DevGpio::J8PinOutC()
{
    DevGpio::J8PinOut();
    return DevGpio::j8PinOut.c_str();
}

gpiod_line* DevGpio::lines[40];
gpiod_chip* DevGpio::chip;

const char* DevGpio::consumer = "devgpio";

string DevGpio::j8PinOut = "";


map<int, std::tuple<int, std::string>> DevGpio::J8 = 
{
    {1, std::make_tuple(-2, "3.3 V")},
    {2, std::make_tuple(-2, "5 V")},
    {3, std::make_tuple(2, "GPIO 2")},
    {4, std::make_tuple(-2, "5 V")},
    {5, std::make_tuple(3, "GPIO 3")},
    {6, std::make_tuple(-1, "GND")},
    {7, std::make_tuple(4, "GPIO 4")},
    {8, std::make_tuple(14, "GPIO 14")},
    {9, std::make_tuple(-1, "GND")},
    {10, std::make_tuple(15, "GPIO 15")},
    {11, std::make_tuple(17, "GPIO 17")},
    {12, std::make_tuple(18, "GPIO 18")},
    {13, std::make_tuple(27, "GPIO 27")},
    {14, std::make_tuple(-1, "GND")},
    {15, std::make_tuple(22, "GPIO 22")},
    {16, std::make_tuple(23, "GPIO 23")},
    {17, std::make_tuple(-2, "3.3 V")},
    {18, std::make_tuple(24, "GPIO 24")},
    {19, std::make_tuple(10, "GPIO 10")},
    {20, std::make_tuple(-1, "GND")},
    {21, std::make_tuple(9, "GPIO 9")},
    {22, std::make_tuple(25, "GPIO 25")},
    {23, std::make_tuple(11, "GPIO 11")},
    {24, std::make_tuple(8, "GPIO 8")},
    {25, std::make_tuple(-1, "GND")},
    {26, std::make_tuple(7, "GPIO 7")},
    {27, std::make_tuple(0, "GPIO 0")},
    {28, std::make_tuple(1, "GPIO 1")},
    {29, std::make_tuple(5, "GPIO 5")},
    {30, std::make_tuple(-1, "GND")},
    {31, std::make_tuple(6, "GPIO 6")},
    {32, std::make_tuple(12, "GPIO 12")},
    {33, std::make_tuple(13, "GPIO 13")},
    {34, std::make_tuple(-1, "GND")},
    {35, std::make_tuple(19, "GPIO 19")},
    {36, std::make_tuple(16, "GPIO 16")},
    {37, std::make_tuple(26, "GPIO 26")},
    {38, std::make_tuple(20, "GPIO 20")},
    {39, std::make_tuple(-1, "GND")},
    {40, std::make_tuple(21, "GPIO 21")}
};