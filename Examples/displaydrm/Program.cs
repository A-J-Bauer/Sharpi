using System.Diagnostics;
using Sharpi;

Display.Drm display = new Display.Drm();

Console.WriteLine(display.Description);

Font font = new Font("DejaVu Sans", 250, Font.Edging.antialias);

Bitmap bitmap = display.GetBitmap();

Canvas canvas = new Canvas(bitmap);

Color color = new Color(0, 255, 255);

Paint paint1 = new Paint();
paint1.Color = new Color(255, 255, 255);

Paint paint2 = new Paint()
{
    Color = new Color(0, 0, 0),
    Stroke = true,
    StrokeWidth = 5.0f,
    AntiAlias = true
};

display.PowerOn();

canvas.Clear(color);
display.Update();

double fps = 0;
string fpsStr = "";
Stopwatch stopwatchFps = new Stopwatch();
stopwatchFps.Start();
for (uint i = 0; i < 1000; i += 10)
{
    fpsStr = $"Aöü: {fps:0.#}";
    canvas.Clear(color);
    canvas.DrawText(fpsStr, i, 300, font, paint1);
    canvas.DrawText(fpsStr, i, 300, font, paint2);
    display.Update();

    fps = 1000.0f / stopwatchFps.ElapsedMilliseconds;
    stopwatchFps.Restart();
}

Thread.Sleep(2000);

Rect rect = font.MeasureText("00:00:00");
float x = (canvas.Width - rect.Width) / 2 - rect.Left;
float y = (canvas.Height - rect.Height) / 2 - rect.Top;

Bitmap testBitmap = new Bitmap("test.png");

DateTime time;
while (!Console.KeyAvailable)
{
    time = DateTime.Now;
    Thread.Sleep((ushort)(1000 - time.Millisecond));
    canvas.Clear(color);
    canvas.DrawBitmap(testBitmap, (bitmap.Width - testBitmap.Width) / 2, (bitmap.Height - testBitmap.Height) / 2);
    canvas.DrawText(time.ToString("HH:mm:ss"), x, y, font, paint1);
    canvas.DrawText(time.ToString("HH:mm:ss"), x, y, font, paint2);
    canvas.DrawLine(0, 0, 1920, 1080, paint1);
    display.Update();
}

display.PowerOff();

paint2.Dispose();
paint1.Dispose();
canvas.Dispose();
bitmap.Dispose();
font.Dispose();
display.Dispose();