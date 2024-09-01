@echo off
setlocal enabledelayedexpansion

set CurrentPath=%cd%
echo Found Bat Script in path: %CurrentPath%.

set PushdCount=0

@REM Loop until the current directory is "Jafg"
:loop
if "%CurrentPath%" == "%cd:Jafg=%" goto found
pushd ..
set CurrentPath=%cd%
set /a PushdCount+=1
goto loop

:found
popd
set /a PushdCount-=1
set CurrentPath=%cd%
echo Found Jafg root directory at: %CurrentPath%. Calling Premake5.

@REM Run the premake5.exe with the specified parameters
Vendor\Premake\Win\premake5.exe --file=Build.lua vs2022

@REM Pop the directories as many times as PushdCount
for /l %%i in (1,1,%PushdCount%) do (
    popd
)

endlocal
