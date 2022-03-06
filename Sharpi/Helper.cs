using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    public static class Helper
    {
        private static readonly string[] BinaryPrefix = { "bytes", "KB", "MB", "GB", "TB" }; // , "PB", "EB", "ZB", "YB"
        public static string GetMemoryString(double bytes)
        {
            int counter = 0;
            double value = bytes;
            string text = "";
            do
            {
                text = value.ToString("0.0") + " " + BinaryPrefix[counter];
                value /= 1024;
                counter++;
            }
            while (Math.Floor(value) > 0 && counter < BinaryPrefix.Length);
            return text;
        }
    }
}
