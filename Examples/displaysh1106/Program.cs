using Sharpi;

// load some bitmaps
Bitmap logo = new Bitmap("logo16x16.bmp");
Bitmap[] alive16x16 = new Bitmap[8];
for (int i = 0; i < alive16x16.Count(); i++)
{
    alive16x16[i] = new Bitmap($"alive{i}_16x10.bmp");
}
Bitmap flatline = new Bitmap("flatline16x10.bmp");
Bitmap voltage8x12 = new Bitmap("voltage8x12.bmp");

// create display
Display.Sh1106 display = new Display.Sh1106();

Console.WriteLine(display.Description);

display.PowerOn();

Font font = new Font("Libertine", 11, Font.Edging.alias);

Paint paint = new Paint()
{
    Color = new Color(255, 255, 255),
};

Bitmap bitmap = display.GetBitmap();

Canvas canvas = new Canvas(bitmap);

Color black = new Color(0, 0, 0);

DateTime Now = DateTime.Now;
int second = Now.Second;
bool[] isAlive = { true, false, true };
bool voltage = true;
string hostname = Info.GetHostname();
int count = 0;

while (!Console.KeyAvailable)
{
    Now = DateTime.Now;

    if (second != Now.Second)
    {
        canvas.Clear(black);
        canvas.DrawBitmap(logo);

        canvas.DrawText("d1", 93, 32, font, paint);
        canvas.DrawBitmap(isAlive[0] ? alive16x16[second % 8] : flatline, 112, 20);

        canvas.DrawText("d2", 93, 48, font, paint);
        canvas.DrawBitmap(isAlive[1] ? alive16x16[second % 8] : flatline, 112, 36);

        canvas.DrawText("d3", 93, 64, font, paint);
        canvas.DrawBitmap(isAlive[2] ? alive16x16[second % 8] : flatline, 112, 52);

        if (voltage)
        {
            canvas.DrawBitmap(voltage8x12, 116, 0);
        }

        canvas.DrawText(hostname, 26, 12, font, paint);
        canvas.DrawText($"{Math.Round(Info.GetTemperature())}°C", 0, 32, font, paint);
        canvas.DrawText(Now.ToString("yyyy-MM-dd"), 0, 48, font, paint);
        canvas.DrawText(Now.ToString("HH:mm:ss"), 0, 64, font, paint);

        if (count < 8)
        {
            bitmap.EncodeToFile($"./screen{count,2:D2}.png", Bitmap.EncodingFormat.PNG);
            count++;
        }

        display.Update();

        second = Now.Second;
    }

    Thread.Sleep(50);
}

display.Dispose();


for (int i = 0; i < alive16x16.Count(); i++)
{
    alive16x16[i].Dispose();
}
flatline.Dispose();
voltage8x12.Dispose();