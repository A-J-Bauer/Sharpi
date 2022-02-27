using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public static partial class Pwm
    {
        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        private static extern string syspwm_get_description();

        [DllImport("sharpi")]
        private static extern void syspwm_set_frequency(int pwm, int frequency);

        [DllImport("sharpi")]
        private static extern void syspwm_set_dutycycle(int pwm, int dutypercent);
        
        [DllImport("sharpi")]
        private static extern void syspwm_create(int pwm);
        
        [DllImport("sharpi")]
        private static extern void syspwm_destroy(int pwm);
        
        [DllImport("sharpi")]
        private static extern void syspwm_power_on(int pwm);
       
        [DllImport("sharpi")]
        private static extern void syspwm_power_off(int pwm);
    }
    
    public static partial class Pwm
    {
        public static string Description
        {
            get
            {
                return syspwm_get_description();
            }
        }

        public static void Create(int pwm)
        {
            syspwm_create(pwm);
        }

        public static void Destroy(int pwm)
        {
            syspwm_destroy(pwm);
        }

        public static void SetFrequency(int pwm, int frequency)
        {
            syspwm_set_frequency(pwm, frequency);
        }

        public static void SetDutyCycle(int pwm, int percent)
        {
            syspwm_set_dutycycle(pwm, percent);
        }

        public static void PowerOn(int pwm)
        {
            syspwm_power_on(pwm);
        }

        public static void PowerOff(int pwm)
        {
            syspwm_power_off(pwm);
        }

    }
}
