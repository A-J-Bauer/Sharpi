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
        // display (common interface)

        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string adc_get_description(IntPtr adc);
        
        [DllImport("sharpi")]
        internal static extern void adc_delete(IntPtr adc);


        // adc (special functions for each type)


        // MCP3008
        [DllImport("sharpi")]
        internal static extern IntPtr adc_mcp3008_new();

        [DllImport("sharpi")]
        internal static extern IntPtr adc_mcp3008_new_x([MarshalAs(UnmanagedType.LPStr)] string spiDevice);

        [DllImport("sharpi")]
        internal static extern IntPtr adc_mcp3008_new_x2([MarshalAs(UnmanagedType.LPStr)] string spiDevice, int spiSpeedHz);

        [DllImport("sharpi")]
        internal static extern int adc_mcp3008_read(IntPtr handle, int channel);

    }

    // adc

    public abstract class AdcBase : IDisposable
    {
        protected IntPtr _handle = IntPtr.Zero;

        public AdcBase(IntPtr handle)
        {
            _handle = handle;
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.adc_delete(_handle);
                _handle = IntPtr.Zero;
            }
        }

        public string Description
        {
            get
            {
                if (_handle != IntPtr.Zero)
                {
                    return Native.adc_get_description(_handle);
                }
                else
                {
                    return "";
                }
            }
        }
    }


    public static class Adc
    {
        public class Mcp3008 : AdcBase
        {
            /// <summary>
            /// MCP3008 analog digital converter
            /// </summary>
            public Mcp3008()
                : base(Native.adc_mcp3008_new())
            {
            }

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="spiDevice">/dev/spidev0.0 or /dev/spidev1.0</param>                
            public Mcp3008(string spiDevice)
            : base(Native.adc_mcp3008_new_x(spiDevice))
            {
            }

            public Mcp3008(string spiDevice, int spiSpeedHz)
                : base(Native.adc_mcp3008_new_x2(spiDevice, spiSpeedHz))
            {
            }

            /// <summary>
            /// Read a value in [0,..,1023 (vref)] from a channel [0,..,7] 
            /// </summary>
            /// <param name="channel">The channel to read from 0,..7</param>
            /// <returns></returns>
            public int Read(byte channel)
            {
                return Native.adc_mcp3008_read(_handle, channel);
            }

        }
    }
}
