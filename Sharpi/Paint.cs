using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;

namespace Sharpi
{
    internal static partial class Native
    {
       
        // SKIA

        [DllImport("sharpi")]
        internal static extern uint skcolor_from_argb(byte r, byte g, byte b, byte a);


        // paint

        [DllImport("sharpi")]
        internal static extern IntPtr skpaint_new();

        [DllImport("sharpi")]
        internal static extern void skpaint_set_color(IntPtr paint, uint color);

        [DllImport("sharpi")]
        internal static extern uint skpaint_get_color(IntPtr paint);

        [DllImport("sharpi")]
        internal static extern void skpaint_set_stroke(IntPtr paint, bool stroke);

        [DllImport("sharpi")]
        internal static extern void skpaint_set_stroke_width(IntPtr paint, float width);

        [DllImport("sharpi")]
        internal static extern float skpaint_get_stroke_width(IntPtr paint);

        [DllImport("sharpi")]
        internal static extern void skpaint_set_antialias(IntPtr paint, bool antialias);

        [DllImport("sharpi")]
        internal static extern void skpaint_delete(IntPtr paint);
    }



  

   

    public class Paint : IDisposable
    {
        internal IntPtr handle = IntPtr.Zero;

        public Paint()
        {
            handle = Native.skpaint_new();
        }


        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                Native.skpaint_delete(handle);
                handle = IntPtr.Zero;
            }
        }

        private Color color = new Color();

        public Color Color
        {
            get
            {
                color.Argb8888 = Native.skpaint_get_color(handle);
                return color;
            }
            set
            {
                color.Argb8888 = value.Argb8888;
                Native.skpaint_set_color(handle, color.Argb8888);
            }
        }

        public bool Stroke
        {
            set
            {
                Native.skpaint_set_stroke(handle, value);
            }
        }

        public float StrokeWidth
        {
            get
            {
                return Native.skpaint_get_stroke_width(handle);
            }

            set
            {
                Native.skpaint_set_stroke_width(handle, value);
            }
        }


        public bool AntiAlias
        {
            set
            {
                Native.skpaint_set_antialias(handle, value);
            }
        }
    }
   
   

}
