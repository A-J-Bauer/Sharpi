using System.Runtime.InteropServices;

namespace Sharpi
{
    public static partial class Gpio
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct Seq
        {
            public long nano;
            public byte value;
        };

        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string devgpio_get_pinout();

        [DllImport("sharpi")]
        private static extern bool devgpio_set_pinmode(int pin, int mode);

        [DllImport("sharpi")]
        private static extern int devgpio_get_pinmode(int pin);

        [DllImport("sharpi")]
        private static extern void devgpio_digitalwrite(int pin, bool value);

        [DllImport("sharpi")]
        private static extern bool devgpio_digitalwrite_sequence(int pin, [In] Seq[] seq, int length);                                   

        [DllImport("sharpi")]
        private static extern bool devgpio_digitalread(int pin);
    }


    /// <summary>
    /// The Gpio class gives you easy access to the pins of your Raspberry Pi.
    /// This library uses physical pin numbering for easy hardware wiring.
    /// Left column pins 1,3,..,39 and right column pins 2,4,..,40
    /// </summary>

    public static partial class Gpio
    {
        public enum Mode : int
        {
            Input = 0,
            Output = 1,
            InputPullUp = 2,
            InputPullDown = 3,
            /// <summary>to read the current value, set the pin mode to Input</summary>            
            InputUnknown = 4,
        }

        public static string PinOut
        {
            get
            {                
                return devgpio_get_pinout();
            }
        }

        public static void SetPinMode(int pin, Mode mode)
        {
            devgpio_set_pinmode(pin, (int)mode);
        }

        public static Mode GetPinMode(int pin)
        {
            return (Mode)devgpio_get_pinmode(pin);
        }

        public static void DigitalWrite(int pin, bool value)
        {
            devgpio_digitalwrite(pin, value);
        }
        public static bool DigitalWriteSequence(int pin, List<ValueTuple<long, bool>> sequence)
        {
            Seq[] seq = new Seq[sequence.Count];
            for (int i=0; i<sequence.Count; i++)
            {
                seq[i].nano = sequence[i].Item1;
                seq[i].value = (byte)(sequence[i].Item2 ? 1 : 0);
            }

            return devgpio_digitalwrite_sequence(pin, seq, seq.Length);
        }

        public static bool DigitalRead(int pin)
        {
            return devgpio_digitalread(pin);
        }

        public static bool DigitalReadDebounced(int pin)
        {
            int value = 0x0;
            
            for (int i=0; i<12; i++)
            {
                value <<= 1;
                value |= (DigitalRead(pin) ? 0x1: 0x0);
                Thread.Sleep(1);                
            }

            return (value & 0b111111) == 0b111111;
        }
    }
}
