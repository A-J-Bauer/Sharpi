using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public static class Sender
    {
        public class Ir28khz : IDisposable
        {
            private const long NEC_BURST = 562500L;

            private List<ValueTuple<long, bool>> repeat = new List<ValueTuple<long, bool>>()
            {                
                new(NEC_BURST * 16, true), // 9ms leading burst
                new(NEC_BURST * 4, false), // 2.25ms space                 
                new(NEC_BURST, true), // end burst
                new(NEC_BURST * 171, false)
            };

            private int _pwm;
            private int _pin;

            public Ir28khz(int pwm, int pin)
            {
                _pwm = pwm;
                _pin = pin;

                if (_pwm == 0 || _pwm == 1)
                {
                    Pwm.Create(_pwm);
                    Pwm.SetFrequency(0, 38000);
                    Pwm.SetDutyCycle(0, 25);
                }                
            }

            public string Description { get { return Pwm.Description; } }

            public void PowerOn()
            {
                if (_pwm == 0 || _pwm == 1)
                {
                    Pwm.PowerOn(_pwm);
                }                
            }

            public void PowerOff()
            {
                if (_pwm == 0 || _pwm == 1)
                {
                    Pwm.PowerOff(_pwm);
                }
            }

            public void Send(ushort address, ushort command)
            {
                List<ValueTuple<long, bool>> sequence = new List<ValueTuple<long, bool>>();

                sequence.Add(new(NEC_BURST * 16, true)); // 9ms leading burst
                sequence.Add(new(NEC_BURST * 8, false)); // 4.5ms space 

                // address extended normal
                ushort bit = 1;
                for (int i = 0; i < 16; i++)
                {
                    sequence.Add(new(NEC_BURST, true));
                    sequence.Add((address & bit) == bit ? new(NEC_BURST * 3, false) : new(NEC_BURST, false));
                    bit <<= 1;
                }

                // command normal
                bit = 1;
                for (int i = 0; i < 8; i++)
                {
                    sequence.Add(new(NEC_BURST, true));
                    sequence.Add((command & bit) == bit ? new(NEC_BURST * 3, false) : new(NEC_BURST, false));
                    bit <<= 1;
                }

                // command inverse
                bit = 1;
                for (int i = 0; i < 8; i++)
                {
                    sequence.Add(new(NEC_BURST, true));
                    sequence.Add((command & bit) == bit ? new(NEC_BURST, false) : new(NEC_BURST * 3, false));
                    bit <<= 1;
                }
                
                sequence.Add(new(NEC_BURST, true)); // end burst
                sequence.Add(new(NEC_BURST * 72, false));

                Gpio.DigitalWriteSequence(_pin, sequence);                
            }

            public void Repeat()
            {
                Gpio.DigitalWriteSequence(_pin, repeat);
            }

            public void Dispose()
            {
                if (_pwm == 0 || _pwm == 1)
                {
                    Gpio.DigitalWrite(_pin, false);
                    Pwm.Destroy(_pwm);
                }
                
            }
        }
    }
}
