:: CHANGE TO YOUR REMOTE FOLDER
set remotefolder="pi@raspberrypi:projects/buzzerpassive/"

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


