using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public partial class UsbWorker
    {
        private delegate void DataCallbackDelegate(int deviceId, [MarshalAs(UnmanagedType.LPStr)] string data);
        private delegate void StateCallbackDelegate(int deviceId, int state);

        [DllImport("sharpi")]
        private static extern IntPtr usb_worker_new(int deviceId, int baud, int deadAfterMs, DataCallbackDelegate datacallback, StateCallbackDelegate statecallback);

        [DllImport("sharpi")]
        private static extern void usb_worker_write(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string data);

        [DllImport("sharpi")]
        private static extern void usb_worker_delete(IntPtr usbworker);
    }


    public partial class UsbWorker : IDisposable
    {
        static string _description =
            "                                                   " + Environment.NewLine +
            "                                     -----------   " + Environment.NewLine +
            "    ----------------------     -----|  Arduino  |  " + Environment.NewLine +
            "   |                      |   |      -----------   " + Environment.NewLine +
            "   |    Raspberry     USB |---       -----------   " + Environment.NewLine +
            "   |                  USB |---------|  Arduino  |  " + Environment.NewLine +
            "   |        Pi        USB |---       -----------   " + Environment.NewLine +
            "   |                      |   |      -----------   " + Environment.NewLine +
            "    ----------------------     -----|  Arduino  |  " + Environment.NewLine +
            "                                     -----------   " + Environment.NewLine +
            "                                                   " + Environment.NewLine;

        private DataCallbackDelegate dataCallback;
        private StateCallbackDelegate stateCallback;

        internal IntPtr handle = IntPtr.Zero;

        private State _state = State.Dead;
        private int _deviceId;

        public enum State : int
        {
            Dead = 0,
            Alive = 1
        }

        public enum Baud : int
        {
            B300 = 7,
            B600 = 8,
            B1200 = 9,
            B1800 = 10,
            B2400 = 11,
            B4800 = 12,
            B9600 = 13,
            B19200 = 14,
            B38400 = 15,
            B57600 = 4097,
            B115200 = 4098
        }

        public class DataEventArgs
        {
            public DataEventArgs(int deviceId, string data) { DeviceId = deviceId; Data = data; }
            public int DeviceId { get; }
            public string Data { get; }
        }
        public delegate void DataDelegate(object sender, DataEventArgs e);
        public event DataDelegate? NewData;

        public class StateEventArgs
        {
            public StateEventArgs(int deviceId, State state) { DeviceId = deviceId; State = state; }
            public int DeviceId { get; }
            public State State { get; }
        }
        public delegate void StateDelegate(object sender, StateEventArgs e);
        public event StateDelegate? StateChanged;

        private void DataCallback(int deviceId, string data)
        {
            if (_deviceId == deviceId)
            {
                NewData?.Invoke(this, new DataEventArgs(_deviceId, data));
            }
        }

        private void StateCallback(int deviceId, int state)
        {
            if (_deviceId == deviceId)
            {
                _state = (State)state;
                StateChanged?.Invoke(this, new StateEventArgs(_deviceId, _state));
            }
        }

        public UsbWorker(int deviceId, Baud baud = Baud.B9600, int deadAfterMs = 1000)
        {
            _deviceId = deviceId;
            dataCallback = new DataCallbackDelegate(DataCallback);
            stateCallback = new StateCallbackDelegate(StateCallback);
            handle = usb_worker_new(_deviceId, (int)baud, deadAfterMs, dataCallback, stateCallback);
        }

        public string Description { get { return _description; } }

        public bool IsALive
        {
            get
            {
                return _state == State.Alive;
            }
        }

        public void Write(string data)
        {
            usb_worker_write(handle, data);
        }

        public void WriteLine(string data)
        {
            usb_worker_write(handle, data + Environment.NewLine);
        }

        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                usb_worker_delete(handle);
                handle = IntPtr.Zero;
            }
        }
    }
}
