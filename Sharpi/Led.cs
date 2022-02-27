using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public class Led
    {
        static string _description =
            "                                  " + Environment.NewLine +
            "  pin                     gnd     " + Environment.NewLine +
            "   |              ###      |      " + Environment.NewLine +
            "   |              led      |      " + Environment.NewLine +
            "   |              ###      |      " + Environment.NewLine +
            "   |            + | | -    |      " + Environment.NewLine +
            "   |            + | |      |      " + Environment.NewLine +
            "    ---- 220Ω ----|  ------       " + Environment.NewLine +
            "                                  " + Environment.NewLine;
            

        private int _pin;

        public Led(int pin)
        {
            _pin = pin;

            Gpio.SetPinMode(_pin, Gpio.Mode.Output);
            Gpio.DigitalWrite(_pin, false);
        }

        public string Description { get { return _description; } }

        public void PowerOn()
        {
            Gpio.DigitalWrite(_pin, true);
        }

        public void PowerOff()
        {
            Gpio.DigitalWrite(_pin, false);
        }

    }
}
