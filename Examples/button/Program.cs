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
