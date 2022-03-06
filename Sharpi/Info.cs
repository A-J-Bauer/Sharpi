using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public static partial class Info
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct MemoryInfo
        {
            public ulong Total;
            public ulong VirtualSize;
            public ulong ResidentSize;
            public ulong ResidentShared;
            public ulong ResidentPrivate;
        };

        [DllImport("sharpi")]
        private static extern void info_get_memory(ref MemoryInfo memInfo);        
    }

    public static partial class Info
    {
        static object hostnamelock = new object();
        public static string GetHostname()
        {
            string hostname = "unknown";

            lock (hostnamelock)
            {
                Process process = new Process
                {
                    StartInfo = new ProcessStartInfo
                    {
                        FileName = "hostname",
                        UseShellExecute = false,
                        CreateNoWindow = true,
                        RedirectStandardOutput = true
                    }
                };

                process.Start();

                if (process.StandardOutput != null)
                {                    
                    string? output = process.StandardOutput.ReadLine();
                    if (output != null)
                    {
                        hostname = output;
                    }
                }
            }            

            return hostname;
        }

        static object temperaturelock = new object();

        /// <summary>
        /// The temperature of the SoC
        /// </summary>
        /// <returns>Celsius</returns>
        public static float GetTemperature()
        {
            float temperature = float.NaN;

            lock (temperaturelock)
            {
                using (FileStream fileStream = new FileStream("/sys/class/thermal/thermal_zone0/temp", FileMode.Open, FileAccess.Read))
                {
                    if (fileStream != null)
                    {
                        using (StreamReader reader = new StreamReader(fileStream))
                        {
                            string? data = reader.ReadLine();
                            if (data?.Length > 0 && int.TryParse(data, out int temp))
                            {
                                temperature = temp / 1000F;
                            }
                        }
                    }
                }
            }
            
            return temperature;
        }

        private static bool cpuInfoCollected = false;
        private static string _hardware = "";
        private static string _revision = "";
        private static string _serial = "";
        private static string _model = "";

        static object cpuinfolock = new object();

        private static void CollectCpuInfo()
        {
            if (cpuInfoCollected)
            {
                return;
            }

            lock (cpuinfolock)
            {
                if (!cpuInfoCollected)
                {
                    using (FileStream fileStream = new FileStream("/proc/cpuinfo", FileMode.Open, FileAccess.Read))
                    {
                        if (fileStream != null)
                        {
                            using (StreamReader reader = new StreamReader(fileStream))
                            {                                
                                string? line = null;
                                string[] lineSplit;

                                while ((line = reader.ReadLine()) != null)
                                {
                                    lineSplit = line.Split(':', StringSplitOptions.TrimEntries | StringSplitOptions.RemoveEmptyEntries);
                                    if (lineSplit.Length == 2)
                                    {
                                        switch (lineSplit[0])
                                        {
                                            case "Hardware":
                                                _hardware = lineSplit[1];
                                                break;
                                            case "Revision":
                                                _revision = lineSplit[1]; ;
                                                break;
                                            case "Serial":
                                                _serial = lineSplit[1]; ;
                                                break;
                                            case "Model":
                                                _model = lineSplit[1]; ;
                                                break;
                                            default:
                                                break;
                                        }
                                    }                                    
                                }

                                cpuInfoCollected = _hardware != "" || _revision != "" || _serial != "" || _model != "";
                            }
                        }
                    }
                }                    
            }
        }
        
        public static string SoC
        {
            get 
            {
                CollectCpuInfo();
                return _hardware; 
            }
        }

        public static string Revision
        {
            get
            {
                CollectCpuInfo();
                return _revision;
            }
        }

        public static string Serial
        {
            get
            {
                CollectCpuInfo();
                return _serial;
            }
        }

        public static string Model
        {
            get
            {
                CollectCpuInfo();
                return _model;
            }
        }

        public static MemoryInfo GetMemoryInfo()
        {
            MemoryInfo memoryInfo = new MemoryInfo();
            info_get_memory(ref memoryInfo);
            return memoryInfo;
        }
    }
}
