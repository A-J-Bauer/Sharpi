<img src="Sharpi/img/sharpi.png">

# Sharpi

C# Library for 64 bit Raspberry Pi OS (aarch64)

- Older Pi models are/will not be supported
- Legacy raspbian or 32 bit versions are/will not be supported
- Scripting languages are/will not be supported 
- Physical header pin numbers 1,2,..,40 are used

# <a name="classtable">
| Class|Description
| ----------- | ----------- |
| [Led](#led) | A simple red, green, blue, yellow or white led
| [Buzzer.Active](#buzzeractive) | Active buzzer (fixed frequency)
| [Buzzer.Passive](#buzzerpassive)| Passive buzzer (variable frequency)
| [Display.Tm1637](#displaytm1637)| A 4-digit 7-segment display
| [Display.Pcd8544](#displaypcd8544)| PCD8544 also known as Nokia screen
| [Display.Ssd1351](#displayssd1351)| SSD1351 Oled 128x128 screen
| [Display.Drm](#displaydrm)| Direct Rendering Manager DRM Display
| [Adc.Mcp3008](#adcmcp3008)| MCP3008 analog digital converter
| [Button](#button)| A debounced button with events
| [UsbWorker](#usbworker)| A serial connector for hotplugging Arduinos
| [Info](#info)| System information class
| [Sensor.Amg8833](#sensoramg8833)| AMG8833 infrared array sensor

<br/>

## <a name="led"></a>Led

A simple light emitting diode

```
 pin                     gnd
   |              ###      |
   |              led      |
   |              ###      |
   |            + | | -    |
   |            + | |      |
    ---- 220Ω ----|  ------
```

Example:<br/>
Power on the led connected to pin 11 (put in series 
to a 220 Ω current limiting resistor and ground) for 1 second.

```csharp
using Sharpi;

Led led = new Led(11);

Console.WriteLine(led.Description);

led.PowerOn();
Thread.Sleep(1000);
led.PowerOff();
```

[Back to list](#classtable)

<br/>

## <a name="buzzeractive"></a>Buzzer Active

Active buzzer with fixed (built in) frequency

```
  gnd     #####     pin
   |      buzzr      |
   |      #####      |
   |       | |       |
    -------   -------
```


Example: Beep for one second.

```csharp
using Sharpi;

Buzzer.Active active = new Buzzer.Active(11);

Console.WriteLine(active.Description);

active.PowerOn();
Thread.Sleep(1000);
active.PowerOff();
```

[Back to list](#classtable)

<br/>

## <a name="buzzerpassive"></a>Buzzer Passive

Passive buzzer with variable frequency

```
  gnd   vcc     #####     pwm
   |     |      buzzr     pin
   |     |      #####      |
   |     |      | | |      |
   |     -------  | |      |
    --------------   ------
```

Example:<br/>
Play some music on a passive piezo buzzer connected to the PWM0 pin.

```csharp
using Sharpi;

List<ValueTuple<int, int>> valkyre = new List<(int, int)>()
{
    (247, 150), (185, 75), (247, 150), (294, 448), (247, 448),(294, 150), (175, 75), (294, 150), 
    (370, 448), (294, 448), (370, 150), (294, 75), (370, 150), (440, 448 ), (220, 448), (294, 150), 
    (220, 75), (294, 150), (370, 732), (175, 150), (294, 150), (175, 75), (294, 150), (370, 448), 
    (294, 448), (370, 150), (294, 75), (370, 150), (440, 448), (370, 448), (440, 150), (370, 75),
    (440, 150), (554, 448), (277, 448), (370, 150), (277, 75), (370, 150), (440, 448)
};

Buzzer.Passive passive = new Buzzer.Passive(0);

Console.WriteLine(passive.Description);

passive.DutyPercent = 50;
passive.PowerOn();

for (int i=0; i< valkyre.Count(); i++)
{
    passive.Frequency = valkyre[i].Item1;    
    Thread.Sleep(valkyre[i].Item2);    
}

passive.PowerOff();
passive.Dispose();
```
[Back to list](#classtable)

<br/>

## <a name="displaytm1637"></a>Display TM1637

A 4-digit 7-segment display

```
  _   _     _   _
 |_| |_| # |_| |_|
 |_| |_| # |_| |_|

TM1637 7 segment LCD Titan Micro Elec.

             1   2
             3   4
             5   6  -- gnd
             7   8
     gnd --  9   10
            11   12
            13   14 -- gnd
     vcc -- 15   16 -- dio
            17   18 -- clk
            19   20
            21   22
            23   24
     gnd -- 25   26
            27   28
            29   30
            31   32
            33   34
            35   36
            37   38
            39   40

supported characters:
  ":'-0123456789ACEFLOPSbdr°
```

Example:<br/>
Rotate the display 180 degrees, set brightness level, show text.

```csharp
using Sharpi;

Display.Tm1637 display = new Display.Tm1637();

Console.WriteLine(display.Description);

display.SetRotation(true);
display.SetBrightness(2);

display.SetText("20°C");
Thread.Sleep(1000);

display.SetText("11:30");
Thread.Sleep(1000);

display.SetText("1234");
Thread.Sleep(1000);

display.SetText("  42");
Thread.Sleep(1000);

display.SetText("42  ");
Thread.Sleep(1000);

display.Dispose();
```

[Back to list](#classtable)

<br/>

## <a name="displaypcd8544"></a>Display PCD8544

PCD8544 also known as Nokia screen

```
           84
   -------------------
  |                   |
  |  ---------------  |
  | |               | |
  | |               | |  48
  | |               | |
  | |               | |
  |  ---------------  |
   -------------------

PCD8544 also known as Nokia screen

config:

  edit /boot/config.txt
  dtparam=spi=on

wiring:

             1   2
             3   4
             5   6  -- gnd
             7   8
     gnd --  9   10
            11   12
            13   14 -- gnd
     vcc -- 15   16 -- dc
            17   18 -- rst
     din -- 19   20
            21   22
     clk -- 23   24 -- ce
     gnd -- 25   26
            27   28
            29   30
            31   32
            33   34
            35   36
            37   38
            39   40
```

Example:<br/>
Output 'Hello' on the screen using the Nimbus font.

```csharp
using Sharpi;


Display.Pcd8544 display = new Display.Pcd8544();

Console.WriteLine(display.Description);

Font font = new Font("Nimbus", 20, Font.Edging.alias);

Bitmap bitmap = display.GetBitmap();

Canvas canvas = new Canvas(display.GetBitmap());

Color color = new Color(255, 255, 255);

Paint paint = new Paint();
paint.Color = new Color(0, 0, 0);

display.PowerOn();

canvas.Clear(color);

canvas.DrawText("Hello", 15, 30, font, paint);

display.Update();

Thread.Sleep(5000);

display.PowerOff();

canvas.Dispose();
bitmap.Dispose();
font.Dispose();
display.Dispose();
```

[Back to list](#classtable)

<br/>

## <a name="displayssd1351"></a>Display SSD1351

SSD1351 Oled 128x128 screen

```
           128
   --------------------    
  | |                | |
  | |                | |
  | |                | |  128
  | |                | |
  | |                | |
  | |                | |
   --------------------

SSD1351 Oled 128x128 screen             
                                        
config:                                 
                                        
  edit /boot/config.txt                 
  dtparam=spi=on                        
  dtoverlay=spi1-1cs (optional for 2nd SPI)
                                        
wiring:                                                                         
                                        
  _0 for SPI 0 and _1 for SPI 1         
                                        
     3.3v -- 1   2  -- 5v               
             3   4  -- 5v               
             5   6  -- gnd              
             7   8                      
     gnd --  9   10                     
            11   12 -- cs_1             
   rst_1 -- 13   14 -- gnd              
    dc_1 -- 15   16 -- dc_0             
            17   18 -- rst_0            
  din_0 --  19   20                     
            21   22                     
   clk_0 -- 23   24 -- cs_0             
     gnd -- 25   26                     
            27   28                     
            29   30                     
            31   32                     
            33   34                     
            35   36                     
            37   38 -- din_1            
            39   40 -- clk_1            
```

Example:<br/>
Show a bitmap loaded from a file and scroll the names of available fonts.

```csharp
using Sharpi;

Console.WriteLine(display.Description);

Font font = new Font("Noto Mono", 20, Font.Edging.alias);

// get the list of font families on the system
List<string> fontNames = Font.Names();

// create a bitmap to draw all fontnames on
Bitmap fontsScrollBitmap = new Bitmap(128, fontNames.Count * 14);

// load a splash bitmap from file
Bitmap splashBitmap = new Bitmap("splash.png"); 

// create a canvas for the fonts bitmap
Canvas fontsCanvas = new Canvas(fontsScrollBitmap);

Paint fontPaint = new Paint()
{
    Color = new Color(0, 255, 255),    
};

// draw the font names on the fontCanvas (thereby changing the underlying bitmap)
for (int i = 0, y = 12; i < fontNames.Count; i++, y += 14)
{
    using (Font fontFont = new Font(fontNames[i], 12, Font.Edging.alias))
    {       
        fontsCanvas.DrawText(fontNames[i], 0, y, fontFont, fontPaint);
    }    
}

// get the display's bitmap
Bitmap bitmap = display.GetBitmap();

// create a canvas for the display's bitmap to draw onto it
Canvas canvas = new Canvas(bitmap);

Color black = new Color(0, 0, 0);

display.PowerOn();

// clear screen, draw the splash bitmap, update display
canvas.Clear(black);
canvas.DrawBitmap(splashBitmap);
display.Update();

Thread.Sleep(2000); // or await Task.Delay(2000);

// scroll the fonts bitmap
Rect src = new Rect(0, 0, 128, 128);
Rect dst = new Rect(0, 0, 128, 128);

for (int y=0; y<fontsScrollBitmap.Height- 128; y++)
{
    canvas.Clear(black);
    src.Top = y;
    src.Bottom = y + 128;
    canvas.DrawBitmap(fontsScrollBitmap, src, dst);
    display.Update();
    Thread.Sleep(10);    
}

Thread.Sleep(1000);

canvas.Dispose();
bitmap.Dispose();
fontPaint.Dispose();
fontsCanvas.Dispose();
splashBitmap.Dispose();
fontsScrollBitmap.Dispose();
display.Dispose();
```

[Back to list](#classtable)

<br/>

<br/>

## <a name="displaydrm"></a>Display DRM

Direct Rendering Manager DRM Display



*If you have the desktop installed on the pi this can only work if your window manager is not claiming the display at the moment.
If the desktop is showing, you need to switch to a virtual terminal. If you are using ssh,
you can **ch**ange **v**irtual **t**erminal with `sudo chvt 1` and back to the desktop 
with `sudo chvt 7` or you can use a connected keyboard and key combinations `ctrl+alt+F1` and `ctrl+alt+F7`
to do the same.*


```
                         1920 (variable)
   ----------------------------------------------------    
  | |                                                | |
  | |                                                | |
  | |                                                | |  
  | |                                                | |
  | |                                                | |
  | |                                                | |  1080 (variable)
  | |                                                | |
  | |                                                | |
  | |                                                | |
  | |                                                | |  
  | |                                                | |
   ----------------------------------------------------    

Direct Rendering Manager DRM Display

config:
 nothing to do

wiring:
 connect a monitor (to HDMI)

not working if a window manager is currently claiming the display.                    

```

Example:<br/> 
Output scrolling text with some UTF-8 characters, 
load and display a test bitmap, measure text for positioning and show the
current time with outlined text, draw a test line from top/left to bottom/right.

```csharp
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
    canvas.DrawBitmap(testBitmap, 0, 0);
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
```

[Back to list](#classtable)

<br/>

## <a name="adcmcp3008"></a>Adc Mcp3008

MCP3008 analog to digital converter (resolution 10 bit, 8 channels)

```
------------------------------------------------------------
MCP3008 8-Channel 10-Bit A/D Converter
       Microchip Technology Inc.

config:

  edit /boot/config.txt
  dtparam=spi=on
  dtoverlay=spi1-1cs (optional for SPI 2)

wiring:

       rpi physical pins

  _0 for SPI 0 and _1 for SPI 1

     3.3v -- 1   2  -- 5v
             3   4  -- 5v
             5   6  -- gnd
             7   8
     gnd --  9   10                       --__--
            11   12 -- cs_1       ch0 -- | o    | -- vdd
   rst_1 -- 13   14 -- gnd        ch1 -- |      | -- vref
    dc_1 -- 15   16 -- dc_0       ch2 -- |      | -- a_gnd
            17   18 -- rst_0      ch3 -- |      | -- clk
   din_0 -- 19   20               ch4 -- |      | -- dout
  dout_0 -- 21   22               ch5 -- |      | -- din
   clk_0 -- 23   24 -- cs_0       ch6 -- |      | -- cs
     gnd -- 25   26               ch7 -- |      | -- d_gnd
            27   28                       ------
            29   30
            31   32
            33   34
            35   36
            37   38 -- din_1
            39   40 -- clk_1
------------------------------------------------------------
```

Example:<br/>
Read from channel 0, output the 10 bit value 0,..,1023 and the
voltage corresponding to vref (assumed to be wired to 3.3V)

```csharp
using Sharpi;

Adc.Mcp3008 mcp3008 = new Adc.Mcp3008();

Console.WriteLine(mcp3008.Description);

Console.WriteLine();
while (!Console.KeyAvailable)
{
    Console.CursorTop -= 1;
    int value = mcp3008.Read(0);
    float volts = 3.3f / 1023 * value;
    Console.WriteLine($"Reading {value} from channel 0 <=> {volts}V ");

    Thread.Sleep(1000);
}
```

[Back to list](#classtable)

<br/>

## <a name="button"></a>Button

A debounced button with events

```
      useExtResistor = false

        pin          3v3
         |            |
          --- push ---
             button


       useExtResistor = true

  gnd          pin          3v3
   |            |            |
    --- 10kΩ --- --- push ---
      resistor      button
```

Example:<br/>
Use automatic updates with events and use internal pulldown.

```csharp
using Sharpi;

void Button_Changed(object sender, Button.EventArgs e)
{
    Console.WriteLine($"button on pin {e.Pin} pressed {e.Pressed}");
}

Button button = new Button(8, true);

Console.WriteLine(button.Description);

button.Pressed += Button_Changed;
button.Released += Button_Changed;

while (!Console.KeyAvailable)
{
    Thread.Sleep(500);
}

button.Pressed -= Button_Changed;
button.Released -= Button_Changed;

button.Dispose();
```

[Back to list](#classtable)

<br/>

## <a name="usbworker"></a>UsbWorker

A serial connector for hotplugging Arduinos


```
                                                
                                    ----------- 
   ----------------------     -----|  Arduino  |
  |                      |   |      ----------- 
  |    Raspberry     USB |---       ----------- 
  |                  USB |---------|  Arduino  |
  |        Pi        USB |---       ----------- 
  |                      |   |      ----------- 
   ----------------------     -----|  Arduino  |
                                    ----------- 

```

Example Arduino:<br/>

Send ULC in the loop to identify the device, listen to commands
"on" or "off", turn on or off the onboard led and send "Led on" / "Led off" accordingly.
```cpp

#define DEVICEID 1

#define STR_H(x) #x
#define STR(x) STR_H(x)
#define ULC "\xF" STR(DEVICEID) "\xE"

// ------------------------ UsbWorker --------------------------
// 1. set DEVICEID to a unique positive integer value  1, 2,.. 
//
// 2. prepend your data with ULC or send ULC alone.
//
//    example: 
//      Serial.print(ULC); Serial.println("my data 12345");
//    or:
//      Serial.println(ULC);
//      
// 3. if no ULC is sent within x milliseconds (default 1500)
//    the device is considered dead (failure or unplugged)
// -------------------------------------------------------------

void setup() 
{
  Serial.begin(38400);  
  
  pinMode(LED_BUILTIN, OUTPUT);  
}

String cmd = "";

void loop() 
{
    while (Serial.available() > 0)
    {
        char cmdChar = Serial.read();        
        
        if (cmdChar == '\n') 
        {
            if (cmd == "on") 
            {
                digitalWrite(LED_BUILTIN, HIGH);
                Serial.print(ULC); Serial.println("Led on");
            }
            else if (cmd == "off")
            {
                digitalWrite(LED_BUILTIN, LOW);
                Serial.print(ULC); Serial.println("Led off");
            }
            else
            {
                Serial.print(ULC); Serial.println("unknown command");
            }

            cmd="";
        }
        else 
        {
            cmd += cmdChar;
        }
  }
  
  Serial.println(ULC);
  
  delay(100);
}

```
Example:<br/>
Add two UsbWorkers, add event handlers and wait for user commands.

```csharp
using Sharpi;

void UsbWorker_StateChanged(object sender, UsbWorker.StateEventArgs e)
{
    Console.WriteLine($"device:{e.DeviceId} state:{e.State.ToString()}");
}

void UsbWorker_NewData(object sender, UsbWorker.DataEventArgs e)
{
    Console.WriteLine($"device:{e.DeviceId} data:{e.Data}");
}

UsbWorker usbWorker1 = new UsbWorker(1, UsbWorker.Baud.B38400);
UsbWorker usbWorker2 = new UsbWorker(2, UsbWorker.Baud.B38400);


usbWorker1.StateChanged += UsbWorker_StateChanged;
usbWorker1.NewData += UsbWorker_NewData;

usbWorker2.StateChanged += UsbWorker_StateChanged;
usbWorker2.NewData += UsbWorker_NewData;


Console.WriteLine("- plug in/out devices to test state changes");
Console.WriteLine("- type command and enter:  \"on 1\", \"off 1\", \"on 2\", \"off 2\", \"q\"");

string? cmd = "";
while (cmd != "q")
{
    if (Console.KeyAvailable)
    {
        cmd = Console.ReadLine();
        if (cmd != null)
        {
            switch (cmd)
            {
                case "on 1":
                    usbWorker1.WriteLine("on");
                    break;
                case "off 1":
                    usbWorker1.WriteLine("off");
                    break;
                case "on 2":
                    usbWorker2.WriteLine("on");
                    break;
                case "off 2":
                    usbWorker2.WriteLine("off");
                    break;
            }
        }

    }
    Thread.Sleep(100);
}

usbWorker1.StateChanged -= UsbWorker_StateChanged;
usbWorker1.NewData -= UsbWorker_NewData;

usbWorker2.StateChanged -= UsbWorker_StateChanged;
usbWorker2.NewData -= UsbWorker_NewData;

usbWorker1.Dispose();
usbWorker2.Dispose();
```

[Back to list](#classtable)

<br/>

## <a name="info"></a>Info

System information class

```
Hostname: raspberrypi
SoC     : BCM2835
Model   : Raspberry Pi 4 Model B Rev 1.4

Memory:
TOTAL   :  3886092.0 kB (3.0 gB)
VIRT    :  2985496.0 kB (2915.0 MB)
RES     :    35912.0 kB
SHR     :    29016.0 kB
PRVT    :     6896.0 kB
```

Example:<br/>
Print temperature, hostname, SoC, revision, 
serial number and model name to standard output.
Additionally output memory information in a loop while producing a
test memory leak.

```csharp
using Sharpi;

float temperature = Info.GetTemperature();
string hostname = Info.GetHostname();

Console.WriteLine($"CPU/GPU/SoC temp: {temperature.ToString("F1")} °C");
Console.WriteLine($"CPU/GPU/SoC temp: {(temperature * 1.8f + 32).ToString("F1")} °F");
Console.WriteLine();
Console.WriteLine($"Hostname: {hostname}");
Console.WriteLine($"SoC     : {Info.SoC}");
Console.WriteLine($"Revision: {Info.Revision}");
Console.WriteLine($"Serial  : {Info.Serial}");
Console.WriteLine($"Model   : {Info.Model}");
Console.WriteLine();

for (int i = 0; i < 6; i++) { Console.WriteLine(); }

while (!Console.KeyAvailable)
{
    var memInfo = Info.GetMemoryInfo();

    // produce a mem leak for testing
    Bitmap memleakdummy = new Bitmap(64, 64);

    Console.CursorTop -= 6;
    Console.WriteLine("Memory:");
    Console.WriteLine($"TOTAL   : {memInfo.Total / 1024,10:F1} kB ({memInfo.Total / (1024 * 1024 * 1024),0:F1} gB)");
    Console.WriteLine($"VIRT    : {memInfo.VirtualSize / 1024,10:F1} kB ({memInfo.VirtualSize / (1024 * 1024),0:F1} MB)");
    Console.WriteLine($"RES     : {memInfo.ResidentSize / 1024,10:F1} kB");
    Console.WriteLine($"SHR     : {memInfo.ResidentShared / 1024,10:F1} kB");
    Console.WriteLine($"PRVT    : {memInfo.ResidentPrivate / 1024,10:F1} kB");

    Thread.Sleep(1000);
}
```

[Back to list](#classtable)

<br/>

## <a name="sensoramg8833"></a>Sensor Amg8833

AMG8833 Infrared array sensor also known as Grid-Eye

```
----------------------------------------------
Amg8833 also known as Grid-Eye

config:

  edit /boot/config.txt
  dtparam=i2c_arm=on,i2c_arm_baudrate=400000

wiring:

       rpi physical pins

     vin --  1   2
     sda --  3   4
     scl --  5   6
             7   8
     gnd --  9   10            -------------
   (int) -- 11   12      vin -|             |
            13   14       3v -|     ---     |
            15   16      gnd -|    |   |    |
            17   18      sda -|    | # |    |
            19   20      scl -|     ---     |
            21   22      int -|             |
            23   24            -------------
            25   26
            27   28
            29   30
            31   32
            33   34
            35   36
            37   38
            39   40

----------------------------------------------
```

Example:<br/>
Power on, output description, read thermistor (as temperature), 
set interrupt high, low and small hysteresis. 
Read grid temperatures and interrupts and output in a loop.
Check if interrupt occured using function and output text if true 
(also int pin has falling edge in this case).
```
Thermistor: 22.8125 °C


                        +       +       +       +       +
                        +       +       +       +       +
                        +       +       +       +       +
                +       +       +       +       +       +
+               +       +       +       +       +       +
+               +       +       +       +       +       +
+       +       +       +       +       +       +       +

Interrupt occured

23.25   25.25   24.25   24.75   25.25   25.75   25.25   24.25
21.25   20.55   23.75   26.25   26.75   26.75   26.75   26.75
19.55   20.25   21.55   27.25   27.25   27.75   27.55   27.25
20.75   20.55   24.25   27.75   27.55   28.25   28.25   27.75
25.75   21.55   26.25   28.25   28.75   28.25   28.75   27.55
26.55   23.25   27.25   28.55   28.55   28.55   28.25   27.75
27.55   25.25   28.25   28.25   28.75   28.75   28.55   28.25
28.25   28.25   29.55   28.55   28.75   28.55   28.55   27.75
```

```csharp
using Sharpi;

Sensor.Amg8833 amg8833 = new Sensor.Amg8833(0x69);

amg8833.PowerOn(); // 5.6 mA 

Console.WriteLine(amg8833.Description);

 float thermistor =  amg8833.ReadThermistor();

Console.WriteLine($"Thermistor: {thermistor} °C");
Console.WriteLine();

amg8833.SetInterrupt(Sensor.Amg8833.InterruptMode.Absolute, 26, 10, 1);

while (!Console.KeyAvailable)
{
    bool[,] ints = amg8833.ReadInterrupts();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Console.Write((ints[i, j] ? "+" : " ") + "\t");
        }

        Console.WriteLine();
    }

    Console.WriteLine();

    if (amg8833.CheckInterrupt()) // alternatively the int pin can be checked for falling edge
    {
        Console.WriteLine("Interrupt occured");
    }
    else
    {
        Console.WriteLine("                 ");
    }

    Console.WriteLine();

    float[,] temps = amg8833.ReadTemperatures();

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            Console.Write(temps[i, j] + "\t");
        }

        Console.WriteLine();
    }

    Console.CursorTop -= 19;
    
    Thread.Sleep(100);
}

Console.CursorTop += 19;

amg8833.PowerOff(); // 0.55 mA

amg8833.Dispose();
```

[Back to list](#classtable)

<br/>
