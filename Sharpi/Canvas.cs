using System.Runtime.InteropServices;

namespace Sharpi
{
    internal static partial class Native
    {
        [DllImport("sharpi")]
        internal static extern IntPtr skcanvas_new_from_bitmap(IntPtr bitmap);

        [DllImport("sharpi")]
        internal static extern void skcanvas_delete(IntPtr canvas);

        [DllImport("sharpi")]
        internal static extern void skcanvas_clear(IntPtr canvas, uint color);

        [DllImport("sharpi")]
        internal static extern void skcanvas_draw_text(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string text, uint len, float x, float y, IntPtr font, IntPtr paint);

        [DllImport("sharpi")]
        internal static extern void skcanvas_draw_line(IntPtr handle, float x0, float y0, float x1, float y1, IntPtr paint);

        [DllImport("sharpi")]
        internal static extern void skcanvas_draw_bitmap(IntPtr handle, float x, float y, IntPtr bitmap);

        [DllImport("sharpi")]
        internal static extern void skcanvas_draw_bitmap_rect(IntPtr handle, ref Rect src, ref Rect dst, IntPtr bitmap);


    }

    public class Canvas : IDisposable
    {
        internal IntPtr _handle = IntPtr.Zero;
        internal Bitmap? _bitmap = null;

        internal int _width;
        internal int _height;

        public Canvas(Bitmap bitmap)
        {
            if (bitmap != null)
            {
                _bitmap = bitmap;
                _width = bitmap._width;
                _height = bitmap._height;
                _handle = Native.skcanvas_new_from_bitmap(bitmap.handle);
            }            
        }

        public int Width
        {
            get { return _width; }
        }

        public int Height
        {
            get { return _height; }
        }

        public void Clear(Color color)
        {
            Native.skcanvas_clear(_handle, color.Argb8888);
        }

        public void DrawText(string text, float x, float y, Font font, Paint paint)
        {
            Native.skcanvas_draw_text(_handle, text, (uint)text.Length, x, y, font.handle, paint.handle);
        }

        public void DrawLine(float x0, float y0, float x1, float y1, Paint paint)
        {
            Native.skcanvas_draw_line(_handle, x0, y0, x1, y1, paint.handle);
        }

        public void DrawBitmap(Bitmap bitmap)
        {
            Native.skcanvas_draw_bitmap(_handle, 0, 0, bitmap.handle);
        }

        public void DrawBitmap(Bitmap bitmap, float x, float y)
        {
            Native.skcanvas_draw_bitmap(_handle, x, y, bitmap.handle);
        }
        public void DrawBitmap(Bitmap bitmap, Rect src, Rect dst)
        {
            Native.skcanvas_draw_bitmap_rect(_handle, ref src, ref dst, bitmap.handle);
        }

        public void Dispose()
        {
            if (_handle != IntPtr.Zero)
            {
                Native.skcanvas_delete(_handle);
                _handle = IntPtr.Zero;

                if (_bitmap != null)
                {
                    _bitmap.handle = IntPtr.Zero;
                }                
            }
        }
    }
}
