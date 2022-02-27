using System.Diagnostics;

namespace Sharpi
{
    [DebuggerDisplay("24 bit argb Color({a},{r},{g},{b})")]
    public class Color
    {        
        private uint argb8888 = 0;

        public Color()
        {
        }

        public Color(byte a, byte r, byte g, byte b)
        {           
            argb8888 = ((uint)a << 24) | ((uint)r << 16) | ((uint)g << 8) | ((uint)b << 0);
        }

        public Color(byte r, byte g, byte b)
            : this(255, r, g, b)
        {
        }

        public uint Argb8888
        {
            get
            {
                return argb8888;
            }
            set
            {
                argb8888 = value;               
            }
        }

        public byte A 
        {
            get
            {
                return (byte)((argb8888 & (uint)0xFF000000) >> 24);
            }           
        }

        public byte R
        {
            get
            {
                return (byte)((argb8888 & (uint)0x00FF0000) >> 16);
            }            
        }

        public byte G
        {
            get
            {
                return (byte)((argb8888 & (uint)0x0000FF00) >> 8);
            }           
        }

        public byte B
        {
            get
            {
                return (byte)((argb8888 & (uint)0x000000FF) >> 0);
            }            
        }
    }
}
