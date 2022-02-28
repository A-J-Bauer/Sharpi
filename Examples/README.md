<img src="..//Sharpi/img/sharpi.png">

# Examples

- All examples are intended to be used with 64 bit Visual Studio
- WSL needs to be installed
- Windows Terminal is not needed but highly recommended.

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

The batch file is given the $(TargetDir) from Visual Studio and the localfolder
is put together from it.

The remotefolder can be changed, this is where the executable and all
necessary files are copied to. 

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

