
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

for (int i = 0; i < valkyre.Count(); i++)
{
    passive.Frequency = valkyre[i].Item1;
    Thread.Sleep(valkyre[i].Item2);
}

passive.PowerOff();
passive.Dispose();