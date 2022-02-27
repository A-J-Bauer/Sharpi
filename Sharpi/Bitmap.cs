using System.Runtime.InteropServices;

namespace Sharpi
{
    internal static partial class Native
    {

        [DllImport("sharpi")]
        internal static extern IntPtr skbitmap_new(int width, int height);

        [DllImport("sharpi")]
        internal static extern IntPtr skbitmap_new_from_file([MarshalAs(UnmanagedType.LPStr)] string path, out int widht, out int height);
        [DllImport("sharpi")]
        internal static extern void skbitmap_delete(IntPtr bitmap);
    }

    /// <summary>
    /// Bitmap
    /// </summary>
    public class Bitmap : IDisposable
    {
        internal IntPtr handle = IntPtr.Zero;
        internal int _width;
        internal int _height;
        
        internal Bitmap(IntPtr handle, int width, int height)
        {
            _width = width;
            _height = height;
            this.handle = handle;
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="path">path to an image file (.bmp, .png, .jpg)</param>
        public Bitmap(string path)
        {
            handle = Native.skbitmap_new_from_file(path, out _width, out _height);
        }

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="width">width of the bitmap</param>
        /// <param name="height">height of the bitmap</param>
        public Bitmap(int width, int height)
        {
            _width = width;
            _height = height;
            handle = Native.skbitmap_new(width, height);
        }

        /// <summary>
        /// the width of the bitmap
        /// </summary>
        public int Width
        {
            get { return _width; }            
        }

        /// <summary>
        /// the height of the bitmap
        /// </summary>
        public int Height
        {
            get { return _height; }            
        }

        /// <summary>
        /// dispose
        /// </summary>
        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                Native.skbitmap_delete(handle);
                handle = IntPtr.Zero;
            }
        }
    }
}
