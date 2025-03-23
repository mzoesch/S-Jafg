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
echo Running Program.py ...
%VENV_DIR%\Scripts\python.exe .\Program.py -UpdateCachedData -INVOKE -- --SolutionGenerator GenerateAll

call :pause_if_needed %1

exit /b

REM Function to pause with Enter only if called from .command
:pause_if_needed
IF NOT "%1" == "from_setup_command" (
    echo Press [Enter] to exit.
    pause >nul
)
exit /b
