using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    [StructLayout(LayoutKind.Sequential)]
    public struct Rect
    {
        public float Left;
        public float Top;
        public float Right;
        public float Bottom;

        public Rect(float left, float top, float right, float bottom)
        {
            Left = left;
            Top = top;
            Right = right;
            Bottom = bottom;
        }

        public float Width { get { return Right - Left; } }
        public float Height { get { return Bottom - Top; } }
    }
}
