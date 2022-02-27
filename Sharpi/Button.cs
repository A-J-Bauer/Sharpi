using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public class Button : IDisposable
    {
        static string _description =
            "                                " + Environment.NewLine +
            "      useExtResistor = false    " + Environment.NewLine +
            "                                " + Environment.NewLine +
            "        pin          3v3        " + Environment.NewLine +
            "         |            |         " + Environment.NewLine +
            "          --- push ---          " + Environment.NewLine +
            "             button             " + Environment.NewLine +
            "                                " + Environment.NewLine +            
            "                                " + Environment.NewLine +
            "       useExtResistor = true    " + Environment.NewLine +
            "                                " + Environment.NewLine +
            "  gnd          pin          3v3 " + Environment.NewLine +
            "   |            |            |  " + Environment.NewLine +
            "    --- 10kΩ --- --- push ---   " + Environment.NewLine +
            "      resistor      button      " + Environment.NewLine +
            "                                ";

        private int _pin;        
        private bool _pressed = false;

        private CancellationTokenSource cancellationTokenSource = new CancellationTokenSource();
        
        private Task<bool>? autoUpdateTask = null;

        public class EventArgs
        {
            public EventArgs(int pin, bool pressed) { Pin = pin; Pressed = pressed; }
            public int Pin { get; }
            public bool Pressed { get; }
        }
        public delegate void StateDelegate(object sender, EventArgs e);
        public event StateDelegate? Pressed;
        public event StateDelegate? Released;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="pin">physical pin to be used</param>
        /// <param name="autoUpdate">automatically call Update in a background thread</param>
        /// <param name="useExtResistor">use an external pulldown resistor, false to use the internal pulldown</param>
        /// <param name="interval">update interval in milliseconds</param>
        public Button(int pin, bool autoUpdate = true, bool useExtResistor = false, int interval = 100)
        {
            _pin = pin;
            if (useExtResistor)
            {
                Gpio.SetPinMode(_pin, Gpio.Mode.Input);
            }
            else
            {
                Gpio.SetPinMode(_pin, Gpio.Mode.InputPullDown);
            }
            
            
            if (autoUpdate)
            {                                 
                autoUpdateTask = CheckButton(interval, cancellationTokenSource.Token);                
            }
        }

        public void Dispose()
        {
            if (autoUpdateTask != null)
            {
                cancellationTokenSource.Cancel();                
                autoUpdateTask.Wait();
                if (autoUpdateTask.Result == true)
                {
                    Console.WriteLine("result is true");
                }
            }            
        }


        private async Task<bool> CheckButton(int interval, CancellationToken cancellationToken)
        {
            while (!cancellationToken.IsCancellationRequested)
            {
                Update();
                await Task.Delay(interval, cancellationToken).ContinueWith(dummy => { });
            }
            
            return true;
        }

        public void Update()
        {
            if (_pressed && !Gpio.DigitalReadDebounced(_pin))
            {
                _pressed = false;
                Released?.Invoke(this, new EventArgs(_pin, _pressed));
            } 
            else if (!_pressed && Gpio.DigitalReadDebounced(_pin))
            {
                _pressed = true;
                Pressed?.Invoke(this, new EventArgs(_pin, _pressed));
            }
        }

      

        public bool IsPressed { get { return _pressed; } }
        public string Description { get { return _description; } }
    }
}
