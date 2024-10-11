@REM Copyright mzoesch. All rights reserved.

@REM Always call this script from the root directory of the project.

@echo off
setlocal enabledelayedexpansion

set CurrentPath=%cd%
echo Found called Bat Script in path: %CurrentPath%. Resolving Jafg root directory.

@REM set PushdCount=0

@REM Loop until the current directory is "Jafg". This is kinda stupid as we cannot rename the root dir of the project.
@REM :loop
@REM if "%CurrentPath%" == "%cd:Jafg=%" goto found
@REM pushd ..
@REM set CurrentPath=%cd%
@REM set /a PushdCount+=1
@REM goto loop

@REM :found
@REM popd
@REM set /a PushdCount-=1
@REM set CurrentPath=%cd%

@REM pushd ..
@REM pushd ..

set CurrentPath=%cd%
echo Found Jafg root directory at: %CurrentPath%. Calling Premake5.

@REM Run the premake5.exe with the specified parameters
Programs\Vendor\Premake\Bin\premake5.exe --file=__buildSolution.lua vs2022

@REM Pop the directories as many times as PushdCount
@REM for /l %%i in (1,1,%PushdCount%) do (
@REM     popd
@REM )

@REM popd
@REM popd

endlocal
