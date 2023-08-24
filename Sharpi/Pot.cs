using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public partial class Pot
    {
        // pot (common interface)

        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string pot_get_description(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern void pot_delete(IntPtr sensor);


        // pot (special functions for each type)


        // AMG8833
        [DllImport("sharpi")]
        internal static extern IntPtr pot_ds3502_new(byte i2cAddress);

        [DllImport("sharpi")]
        internal static extern IntPtr pot_ds3502_new_x(byte i2cAddress, [MarshalAs(UnmanagedType.LPStr)] string i2cDevice);


        [DllImport("sharpi")]
        internal static extern void pot_ds3502_power_on(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern void pot_ds3502_power_off(IntPtr sensor);

        [DllImport("sharpi")]
        internal static extern byte pot_ds3502_get_wiper(IntPtr pot);

        [DllImport("sharpi")]
        internal static extern void pot_ds3502_set_wiper(IntPtr pot, byte value);

        [DllImport("sharpi")]
        internal static extern void pot_ds3502_set_wiper_persistent(IntPtr pot, byte value);
    }

    // sensor

    public abstract class PotBase : IDisposable
    {
        protected IntPtr _handle = IntPtr.Zero;

        public PotBase(IntPtr handle)
        {
            _handle = handle;
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                Pot.pot_delete(_handle);
                _handle = IntPtr.Zero;
            }
        }

        public string Description
        {
            get
            {
                if (_handle != IntPtr.Zero)
                {
                    return Pot.pot_get_description(_handle);
                }
                else
                {
                    return "";
                }
            }
        }
    }


    public static partial class Pot
    {
        public class Ds3502 : PotBase
        {
            /// <summary>
            /// Ds3502 digipot, standard i2caddress 0x28
            /// </summary>
            public Ds3502(byte i2caddress)
                : base(pot_ds3502_new(i2caddress))
            {
            }

            public Ds3502(byte i2caddress, string i2cdevice)
                : base(pot_ds3502_new_x(i2caddress, i2cdevice))
            {
            }

            public void PowerOn()
            {
                pot_ds3502_power_on(_handle);
            }

            public void PowerOff()
            {
                pot_ds3502_power_off(_handle);
            }

            public byte GetWiper()
            {
                return pot_ds3502_get_wiper(_handle);
            }

            /// <summary>
            /// sets the wiper value
            /// </summary>
            /// <param name="value">0..127</param>
            public void SetWiper(byte value)
            {
                pot_ds3502_set_wiper(_handle, value);
            }

            /// <summary>
            /// sets the wiper value and stores it in the EEPROM (~50000 writes)
            /// </summary>
            /// <param name="value">0..127</param>
            public void SetWiperPersistent(byte value)
            {
                pot_ds3502_set_wiper_persistent(_handle, value);
            }
        }

    }


}
