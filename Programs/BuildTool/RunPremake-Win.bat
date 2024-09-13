@REM Copyright mzoesch. All rights reserved.

@echo off
setlocal enabledelayedexpansion

set CurrentPath=%cd%
echo Found called Bat Script in path: %CurrentPath%. Resolving Jafg root directory.

set PushdCount=0

@REM Loop until the current directory is "Jafg". This is kinda stupid as we cannot rename the root dir of the project. 
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
Programs\Vendor\Premake\Bin\premake5.exe --file=Build.lua vs2022

@REM Pop the directories as many times as PushdCount
for /l %%i in (1,1,%PushdCount%) do (
    popd
)

endlocal
