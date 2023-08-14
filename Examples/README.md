<img src="../Sharpi/img/sharpi.png">

# Examples

- All examples are intended to be used with 64 bit [Visual Studio](https://visualstudio.microsoft.com/downloads/)
- [WSL](https://docs.microsoft.com/en-us/windows/wsl/install) needs to be installed
- [Windows Terminal](https://github.com/microsoft/terminal) is not needed but highly recommended.

## .csproj
The .csproj file for each 
example is set for ARM64, selfcontained and automatic deployment to the
raspberry using rsync over ssh by running postbuild.bat after a successful build.
Informational only - Nothing to change.
```
<Project Sdk="Microsoft.NET.Sdk">
	<PropertyGroup>
		<OutputType>Exe</OutputType>
		<TargetFramework>net6.0</TargetFramework>
		<ImplicitUsings>enable</ImplicitUsings>
		<Nullable>enable</Nullable>
		<Platforms>ARM64</Platforms>
		<RuntimeIdentifier>linux-arm64</RuntimeIdentifier>
		<SelfContained>True</SelfContained>
		<RunPostBuildEvent>OnBuildSuccess</RunPostBuildEvent>
		<OutputPath>bin\ARM64</OutputPath>
	</PropertyGroup>
	<Target Name="PostBuild" AfterTargets="PostBuildEvent">
		<Exec Command="&quot;$(ProjectDir)postbuild.bat&quot; &quot;$(TargetDir)&quot;"/>
	</Target>
	<ItemGroup>
	  <PackageReference Include="Sharpi" Version="*" />
	</ItemGroup>
</Project>
```

## postbuild.bat

The batch file is given the $(TargetDir) as input from Visual Studio and is transformed into a linux compatible path.
The converted path is used as source for the linux rsync command run by the WSL.

The remotefolder is where the executable and all necessary files are copied into.

`set remotefolder="USER@HOSTNAME:/FOLDER/`

```
:: CHANGE TO YOUR REMOTE FOLDER
set remotefolder="pi@raspberrypi:projects/examplename/"

@echo off
:: argument MSBuild macro $(TargetDir)
set targetdir=%1
:: change backward to forward slashes
set targetdir=%targetdir:\=/%
:: remove double quotes
set targetdir=%targetdir:"=%
:: replace C:, D:, E:, F:, G: lowercase no colon
set targetdir=%targetdir:C:=c:%
set targetdir=%targetdir:D:=d%
set targetdir=%targetdir:E:=e%
set targetdir=%targetdir:F:=f%
set targetdir=%targetdir:G:=g%
:: prepend /mnt/
set targetdir=/mnt/%targetdir%
:: add double quotes
set localfolder="%targetdir%"
echo.

echo rsync folders
echo from:  %localfolder%
echo to:    %remotefolder%
wsl rsync -avzh --timeout=2 --update %localfolder% %remotefolder%
echo done
echo.
```

## ssh, rsync and remote debugging

installing rsync and openssh-client in the WSL distro:
```
sudo apt install rsync openssh-client
```

creating private and public keys in WSL (enter three times):
```
ssh-keygen
```

copying generated keys to the remote host (raspberrypi):
```
ssh-copy-id -i ~/.ssh/id_rsa.pub raspberrypi
```

connecting to the pi (ssh enabled in config):
```
ssh raspberrypi
```

managing remote debugging ssh connections in visual studio:

> Debug/Options/Cross Platform/Connection Manager

create directory in home dir and set permissions for the remote debugger:
```
cd ~
mkdir .vs-debugger
chmod 777 .vs-debugger
```

waiting in a c# program for the debugger to attach:

```csharp
using System.Diagnostics;
while (!Debugger.IsAttached)
{
    Thread.Sleep(1);
}
```

attaching to program (XYZ) running on host (raspberrypi):
> Debug/Attach to process  
> Connection type: SSH  
> Connection target: pi@raspberrypi  
> Filter processes: XYZ  
> Attach to Select: Managed (.NET Core for Unix)

subsequent attaching with same settings / reattaching
> Gear symbol
