@echo off

REM Copyright mzoesch. All rights reserved.

SETLOCAL ENABLEDELAYEDEXPANSION

FOR %%I IN ("%~dp0..") DO SET "JAFG_ROOT=%%~fI"
SET "PROGRAMS_DIR=%JAFG_ROOT%\Programs"

SET "MSYS2_ROOT=%JAFG_ROOT%\msys64"
SET "MSYS2_INSTALLER=%JAFG_ROOT%\msys2-installer.exe"
SET "MSYS2_URL=https://github.com/msys2/msys2-installer/releases/download/2026-06-11/msys2-base-x86_64-20260611.sfx.exe"
SET "MSYS2_SHELL=%MSYS2_ROOT%\ucrt64.exe"

ECHO Jafg root: "%JAFG_ROOT%"

PUSHD "%JAFG_ROOT%" || (
    ECHO Failed to change into Jafg root folder.
    EXIT /B 1
    )

ECHO Generating MSYS2 environment...
IF NOT EXIST "%MSYS2_INSTALLER%" (
    ECHO Downloading MSYS2 installer...
    powershell -NoProfile -Command "Invoke-WebRequest -Uri '%MSYS2_URL%' -OutFile '%MSYS2_INSTALLER%' -UseBasicParsing"
    IF ERRORLEVEL 1 (
        ECHO Downloading MSYS2 installer failed.
        POPD
        EXIT /B 1
        )
) ELSE (
    ECHO MSYS2 installer already exists. Skipping download.
    )

IF NOT EXIST "%MSYS2_ROOT%" (
    ECHO Installing MSYS2...
    "%MSYS2_INSTALLER%" -y -o"%JAFG_ROOT%"
    IF ERRORLEVEL 1 (
        ECHO MSYS2 installer failed!
        POPD
        EXIT /B 1
        )
) ELSE (
    ECHO MSYS2 is already installed. Skipping installation.
    )

IF NOT EXIST "%MSYS2_SHELL%" (
    ECHO "%MSYS2_SHELL%": No such MSYS2 shell.
    POPD
    EXIT /B 1
    )

ECHO Initializing MSYS2 environment...
"%MSYS2_SHELL%" bash "%PROGRAMS_DIR%\msys2_install_pkgs.sh"

IF ERRORLEVEL 1 (
    ECHO MSYS2 setup failed!
    POPD
    EXIT /B 1
    )

ECHO Please wait for MSYS2 UCRT64 to complete installation before continuing.
POPD
ENDLOCAL
PAUSE
