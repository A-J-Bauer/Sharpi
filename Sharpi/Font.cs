using System.Runtime.InteropServices;

namespace Sharpi
{
    internal static partial class Native
    {
        // FONT

        [DllImport("sharpi")]
        [return: MarshalAs(UnmanagedType.LPStr)]
        internal static extern string font_get_fonts();

        [DllImport("sharpi")]
        internal static extern IntPtr skfont_new_from_file([MarshalAs(UnmanagedType.LPStr)] string path, float size, byte edging);

        [DllImport("sharpi")]
        internal static extern IntPtr skfont_new_from_name([MarshalAs(UnmanagedType.LPStr)] string name, float size, byte edging);

        [DllImport("sharpi")]
        internal static extern  void skfont_measure_text(IntPtr handle, [MarshalAs(UnmanagedType.LPStr)] string text, ulong length, ref Rect rect);

        [DllImport("sharpi")]
        internal static extern void skfont_delete(IntPtr font);
    }

    public class Font : IDisposable
    {
        internal IntPtr handle = IntPtr.Zero;

        public enum Edging : byte
        {
            alias = 0,
            antialias = 1,
            subpixelantialias = 2
        }

        public static List<string> Names()
        {
            List<string> available = new List<string>();
            string fonts = Native.font_get_fonts();
            string[] fontsSplit = fonts.Split(new char[] { '\n' }, StringSplitOptions.RemoveEmptyEntries);
            for (int i = 0; i < fontsSplit.Length; i++)
            {
                available.Add(fontsSplit[i]);
            }
            return available;
        }

        public Font(string fontNameOrPath, float size, Edging edging)
        {
            // simple check for now
            if (fontNameOrPath.Contains("."))
            {
                handle = Native.skfont_new_from_file(fontNameOrPath, size, (byte)edging);
            }
            else
            {
                handle = Native.skfont_new_from_name(fontNameOrPath, size, (byte)edging);
            }
        }

        public Rect MeasureText(string text)
        {
            Rect rect = new Rect();
            Native.skfont_measure_text(handle, text, (ulong)text.Length, ref rect);
            return rect;
        }

        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                Native.skfont_delete(handle);
                handle = IntPtr.Zero;
            }
        }
    }
}
