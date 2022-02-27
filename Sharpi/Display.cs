﻿using System;
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
        internal static extern string display_get_description(IntPtr display);

        [DllImport("sharpi")]
        internal static extern bool display_is_on(IntPtr display);

        [DllImport("sharpi")]
        internal static extern void display_power_on(IntPtr display);

        [DllImport("sharpi")]
        internal static extern void display_power_off(IntPtr display);

        [DllImport("sharpi")]
        internal static extern void display_update(IntPtr display);

        [DllImport("sharpi")]
        internal static extern void display_delete(IntPtr display);


        // display (special functions for each type)


        // PCD8544
        [DllImport("sharpi")]
        internal static extern IntPtr display_pcd8544_new();

        [DllImport("sharpi")]
        internal static extern IntPtr display_pcd8544_new_x([MarshalAs(UnmanagedType.LPStr)] string spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin);

        [DllImport("sharpi")]
        internal static extern IntPtr display_pcd8544_get_bitmap(IntPtr display, out int width, out int height);



        // SSD1351
        [DllImport("sharpi")]
        internal static extern IntPtr display_ssd1351_new();

        [DllImport("sharpi")]
        internal static extern IntPtr display_ssd1351_new_x([MarshalAs(UnmanagedType.LPStr)] string spiDevice, byte rotation);
        [DllImport("sharpi")]
        internal static extern IntPtr display_ssd1351_new_x2([MarshalAs(UnmanagedType.LPStr)] string spiDevice, byte rotation, int spiSpeedHz, int dataPin, int resetPin);
        [DllImport("sharpi")]
        internal static extern IntPtr display_ssd1351_get_bitmap(IntPtr display, out int width, out int height);



        // TM1637
        [DllImport("sharpi")]
        internal static extern IntPtr display_tm1637_new();

        [DllImport("sharpi")]
        internal static extern IntPtr display_tm1637_new_x(int powerPin, int dataPin, int clockPin);

        [DllImport("sharpi")]
        internal static extern void display_tm1637_set_brightness(IntPtr display, int zerotofour);

        [DllImport("sharpi")]
        internal static extern void display_tm1637_set_rotation(IntPtr display, bool rotation);

        [DllImport("sharpi")]
        internal static extern void display_tm1637_set_text(IntPtr display, [MarshalAs(UnmanagedType.LPStr)] string text);


        // DRM
        [DllImport("sharpi")]
        internal static extern IntPtr display_drm_new();

        [DllImport("sharpi")]
        internal static extern IntPtr display_drm_get_bitmap(IntPtr display, out int width, out int height);

        //[DllImport("sharpi")]
        //internal static extern ushort display_drm_get_width(IntPtr display);

        //[DllImport("sharpi")]
        //internal static extern ushort display_drm_get_height(IntPtr display);
    }


    // display

    public abstract class DisplayBase : IDisposable
    {
        protected IntPtr _handle = IntPtr.Zero;

        public DisplayBase(IntPtr handle)
        {
            _handle = handle;
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.display_delete(_handle);
                _handle = IntPtr.Zero;
            }
        }

        public string Description
        {
            get
            {
                if (_handle != IntPtr.Zero)
                {
                    return Native.display_get_description(_handle);
                }
                else
                {
                    return "";
                }
            }
        }

        public bool IsOn
        {
            get
            {
                if (_handle != IntPtr.Zero)
                {
                    return Native.display_is_on(_handle);
                }
                else
                {
                    return false;
                }
            }
        }

        public void PowerOn()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.display_power_on(_handle);
            }
        }

        public void PowerOff()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.display_power_off(_handle);
            }
        }

        public void Update()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.display_update(_handle);
            }
        }

    }

    public static class Display
    {
        /// <summary>
        /// PCD8544 also known as Nokia screen
        /// </summary>
        public class Pcd8544 : DisplayBase
        {
            public Pcd8544()
                : base(Native.display_pcd8544_new())
            {
            }

            public Pcd8544(string spiDevice, int spiSpeedHz, int powerPin, int dataPin, int resetPin)
                : base(Native.display_pcd8544_new_x(spiDevice, spiSpeedHz, powerPin, dataPin, resetPin))
            {
            }

            public Bitmap GetBitmap()
            {
                int width;
                int height;
                IntPtr handle = Native.display_pcd8544_get_bitmap(_handle, out width, out height);
                return new Bitmap(handle, width, height);
            }
        }

        /// <summary>
        /// SSD1351 Oled 128x128 screen 
        /// </summary>
        public class Ssd1351 : DisplayBase
        {
            public Ssd1351()
                : base(Native.display_ssd1351_new())
            {
            }

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="spiDevice">/dev/spidev0.0 or /dev/spidev1.0</param>
            /// <param name="rotation">0,..,3</param>
            public Ssd1351(string spiDevice, byte rotation)
                : base(Native.display_ssd1351_new_x(spiDevice, rotation))
            {
            }

            /// <summary>
            /// Constructor
            /// </summary>
            /// <param name="spiDevice">/dev/spidev0.0 or /dev/spidev1.0</param>
            /// <param name="rotation">0,..,3</param>
            /// <param name="spiSpeedHz">spi speed in hz e.g. 24000000</param>
            /// <param name="dataPin">data pin</param>
            /// <param name="resetPin">reset pin</param>
            public Ssd1351(string spiDevice, byte rotation, int spiSpeedHz, int dataPin, int resetPin)
                : base(Native.display_ssd1351_new_x2(spiDevice, rotation, spiSpeedHz, dataPin, resetPin))
            {
            }

            public Bitmap GetBitmap()
            {
                int width;
                int height;
                IntPtr handle = Native.display_ssd1351_get_bitmap(_handle, out width, out height);
                return new Bitmap(handle, width, height);
            }
        }

        /// <summary>
        /// TM1637 7 segment LCD Titan Micro Elec.
        /// </summary>
        public class Tm1637 : DisplayBase
        {
            public Tm1637()
                : base(Native.display_tm1637_new())
            {
            }

            public Tm1637(int powerPin, int dataPin, int clockPin)
                : base(Native.display_tm1637_new_x(powerPin, dataPin, clockPin))
            {
            }



            public void SetBrightness(int zerotofour)
            {
                Native.display_tm1637_set_brightness(_handle, zerotofour);
            }

            public void SetRotation(bool rotation)
            {
                Native.display_tm1637_set_rotation(_handle, rotation);
            }

            public void SetText(string text)
            {
                Native.display_tm1637_set_text(_handle, text);
            }
        }


        public class Drm : DisplayBase
        {
            //private ushort width;
            //private ushort height;

            public Drm()
                : base(Native.display_drm_new())
            {
                //if (_handle != IntPtr.Zero)
                //{
                //    width = Native.display_drm_get_width(_handle);
                //    height = Native.display_drm_get_height(_handle);
                //}
            }

            //public ushort Width 
            //{
            //    get
            //    {
            //        return width;
            //    }
            //}

            //public ushort Height
            //{
            //    get
            //    {
            //        return height;
            //    }
            //}

            public Bitmap GetBitmap()
            {
                int width;
                int height;
                IntPtr handle = (Native.display_drm_get_bitmap(_handle, out width, out height));

                return new Bitmap(handle, width, height);
            }


        }
    }


}
