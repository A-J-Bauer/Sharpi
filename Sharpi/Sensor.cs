using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    internal static partial class Native
    {
        // sensor (common interface)

        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string sensor_get_description(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern void sensor_delete(IntPtr sensor);


        // sensor (special functions for each type)


        // AMG8833
        [DllImport("sharpi")]
        internal static extern IntPtr sensor_amg8833_new(byte i2cAddress);

        [DllImport("sharpi")]
        internal static extern IntPtr sensor_amg8833_new_x(byte i2cAddress, [MarshalAs(UnmanagedType.LPStr)] string i2cDevice);


        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_power_on(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_power_off(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_clear_status(IntPtr sensor, bool overflow, bool interrupt);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_get_status(IntPtr sensor, out bool overflow, out bool interrupt);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_set_moving_average_emode(IntPtr sensor, bool on);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_set_frame_rate(IntPtr sensor, bool high);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_set_interrupt(IntPtr sensor, byte mode);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_set_interrupt_x(IntPtr sensor, byte mode, float highTemp, float lowTemp);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_set_interrupt_x2(IntPtr sensor, byte mode, float highTemp, float lowTemp, float hysteresis);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_readInterrupts(IntPtr sensor, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.U1, SizeConst = 64)] byte[] values);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_read_thermistor_short(IntPtr sensor, out short value);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_read_thermistor_float(IntPtr sensor, out float value);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_read_temperatures_short(IntPtr sensor, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.I2, SizeConst = 64)] short[] values);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_read_temperatures_float(IntPtr sensor, [MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.R4, SizeConst = 64)] float[] values);

        [DllImport("sharpi")]
        internal static extern void sensor_amg8833_update_temperatures_bitmap(IntPtr sensor, float minTemp, float maxTemp);

        [DllImport("sharpi")]
        internal static extern IntPtr sensor_amg8833_get_bitmap(IntPtr display, out int width, out int height);        

    }

    // sensor

    public abstract class SensorBase : IDisposable
    {
        protected IntPtr _handle = IntPtr.Zero;

        public SensorBase(IntPtr handle)
        {
            _handle = handle;
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.sensor_delete(_handle);
                _handle = IntPtr.Zero;
            }
        }

        public string Description
        {
            get
            {
                if (_handle != IntPtr.Zero)
                {
                    return Native.sensor_get_description(_handle);
                }
                else
                {
                    return "";
                }
            }
        }
    }


    public static partial class Sensor
    {
        public class Amg8833 : SensorBase
        {
            public enum InterruptMode : byte
            {
                None = 0,
                Absolute = 1,
                Difference = 2
            }

            /// <summary>
            /// AMG8833 grid-eye
            /// </summary>
            public Amg8833(byte i2caddress)
                : base(Native.sensor_amg8833_new(i2caddress))
            {
            }
          
            public Amg8833(byte i2caddress, string i2cdevice)
                : base(Native.sensor_amg8833_new_x(i2caddress, i2cdevice))
            {
            }

            public void PowerOn()
            {
                Native.sensor_amg8833_power_on(_handle);
            }

            public void PowerOff()
            {
                Native.sensor_amg8833_power_off(_handle);
            }

            //public void ClearStatus(bool overflow, bool interrupt)
            //{
            //    sensor_amg8833_clear_status(_handle, overflow, interrupt);
            //}

            public bool CheckInterrupt()
            {
                bool overflow = false;
                bool interrupt = false;
                Native.sensor_amg8833_get_status(_handle, out overflow, out interrupt);
                return interrupt;
            }

            public bool CheckOverflow()
            {
                bool overflow = false;
                bool interrupt = false;
                Native.sensor_amg8833_get_status(_handle, out overflow, out interrupt);
                return overflow;
            }

            public void SetAverageMode(bool on)
            {
                Native.sensor_amg8833_set_moving_average_emode(_handle, on);
            }

            public void SetFrameRate(bool high)
            {
                Native.sensor_amg8833_set_frame_rate(_handle, high);
            }

            public void SetInterrupt(InterruptMode mode)
            {
                Native.sensor_amg8833_set_interrupt(_handle, (byte)mode);
            }

            public void SetInterrupt(InterruptMode mode, float highTemp, float lowTemp)
            {
                Native.sensor_amg8833_set_interrupt_x(_handle, (byte)mode, highTemp, lowTemp);
            }

            public void SetInterrupt(InterruptMode mode, float highTemp, float lowTemp, float hysteresis)
            {
                Native.sensor_amg8833_set_interrupt_x2(_handle, (byte)mode, highTemp, lowTemp, hysteresis);
            }

            public bool[,] ReadInterrupts()
            {
                bool[,] values = new bool[8,8];
                byte[] nvalues = new byte[64];

                Native.sensor_amg8833_readInterrupts(_handle, nvalues);
                for (int i = 0; i < 8; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        values[i,j] = nvalues[i * 8 + j] == 1;
                    }
                }

                return values;
            }

            public float ReadThermistor()
            {
                float value = 0;
                Native.sensor_amg8833_read_thermistor_float(_handle, out value);
                return value;
            }

            public float[,] ReadTemperatures()
            {
                float[,] values = new float[8,8];
                float[] nvalues = new float[64];

                Native.sensor_amg8833_read_temperatures_float(_handle, nvalues);
                for (int i = 0; i < 8; i++)
                {
                    for (int j = 0; j < 8; j++)
                    {
                        values[i,j] = nvalues[i * 8 + j];
                    }
                }

                return values;
            }

            public Bitmap ReadHeatMap(float minTemp, float maxTemp)
            {
                IntPtr bitmap = IntPtr.Zero;
                Native.sensor_amg8833_update_temperatures_bitmap(_handle, minTemp, maxTemp);
                int width;
                int height;
                IntPtr handle = Native.sensor_amg8833_get_bitmap(_handle, out width, out height);
                return new Bitmap(handle, width, height);
            }

        }
    }


}
