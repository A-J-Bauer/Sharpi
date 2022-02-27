using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
   
    public static class Buzzer
    {
        public abstract class BuzzerBase
        {
            public abstract string Description { get; }
            public abstract void PowerOn();
            public abstract void PowerOff();
        }

        /// <summary>
        /// Passive buzzer with variable frequency
        /// </summary>
        public class Passive : BuzzerBase, IDisposable
        {
            static string _description =
            "                                " + Environment.NewLine +
            "  gnd   vcc     #####     pwm   " + Environment.NewLine +
            "   |     |      buzzr     pin   " + Environment.NewLine +
            "   |     |      #####      |    " + Environment.NewLine +
            "   |     |      | | |      |    " + Environment.NewLine +
            "   |     -------  | |      |    " + Environment.NewLine +
            "    --------------   ------     " + Environment.NewLine +            
            "                                " + Environment.NewLine;

            private int _pwm = -1;
            private int _frequency;
            private int _dutypercent;

            public Passive(int pwm)                
            {
                if (pwm != 0 && pwm != 1)
                {
                    return;
                }

                _pwm = pwm;

                Pwm.Create(_pwm);
            }

            public void Dispose()
            {
                if (_pwm != -1)
                {
                    Pwm.PowerOff(_pwm);
                    Pwm.Destroy(_pwm);
                }                
            }


            public override string Description { get { return Pwm.Description + _description; } }

            public override void PowerOn()
            {
                if (_pwm != -1)
                {
                    Pwm.PowerOn(_pwm);                   
                }
            }

            public override void PowerOff()
            {
                if (_pwm != -1)
                {
                    Pwm.PowerOff(_pwm);
                }
            }

            public int Frequency
            {
                get { return _frequency; }
                set 
                { 
                    _frequency = value;
                    if (_pwm != -1)
                    {
                        Pwm.SetFrequency(_pwm, _frequency);
                    }
                }
            }

            public int DutyPercent
            {
                get { return _dutypercent; }
                set 
                { 
                    _dutypercent = value;
                    if (_pwm != -1)
                    {
                        Pwm.SetDutyCycle(_pwm, _dutypercent);
                    }
                }
            }


        }

        /// <summary>
        /// Active buzzer with fixed (built in) frequency
        /// </summary>
        public class Active : BuzzerBase
        {
            static string _description =
            "                         " + Environment.NewLine +
            "  gnd     #####     pin  " + Environment.NewLine +
            "   |      buzzr      |   " + Environment.NewLine +
            "   |      #####      |   " + Environment.NewLine +
            "   |       | |       |   " + Environment.NewLine +
            "    -------   -------    " + Environment.NewLine +
            "                         " + Environment.NewLine;

            private int _pin;

            public Active(int pin)
            {
                _pin = pin;

                Gpio.SetPinMode(_pin, Gpio.Mode.Output);
                Gpio.DigitalWrite(_pin, false);
            }

            public override string Description { get { return _description; } }

            public override void PowerOn()
            {
                Gpio.DigitalWrite(_pin, true);
            }

            public override void PowerOff()
            {
                Gpio.DigitalWrite(_pin, false);
            }
           
        }
    }
}
