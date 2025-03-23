@echo off
REM Copyright mzoesch. All rights reserved.

REM Set cwd.
cd /d "%~dp0"

set VENV_DIR=.venv

where python3.11 >nul 2>&1
IF %ERRORLEVEL% NEQ 0 (
    echo Python 3.11 is not installed or not in PATH.
    call :pause_if_needed %1
    exit /b 1
)

echo Creating virtual environment (%VENV_DIR%\) ...
python3.11 -m venv %VENV_DIR%

echo Activating virtual environment ...
call %VENV_DIR%\Scripts\activate.bat

echo Virtual environment activated with:
python --version

echo Running Program.py ...
python ./Program.py -UpdateSubmodules -ValidatePython -ValidateRust -ValidatePremake -ValidateCmake -UpdateCachedData -INVOKE -- --SolutionGenerator GenerateAll EmulateCompiler

call :pause_if_needed %1

exit /b

REM Function to pause with Enter only if called from .command
:pause_if_needed
IF NOT "%1" == "from_setup_command" (
    echo Press [Enter] to exit.
    pause >nul
)
exit /b
