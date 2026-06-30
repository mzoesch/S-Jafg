@echo off

REM Copyright mzoesch. All rights reserved.

SETLOCAL ENABLEDELAYEDEXPANSION

SET "MSYS2_ROOT=%~dp0msys64"
SET "MSYS2_INSTALLER=%~dp0msys2-installer.exe"
SET "MSYS2_URL=https://github.com/msys2/msys2-installer/releases/download/2026-06-11/msys2-base-x86_64-20260611.sfx.exe"
SET "MSYS2_SHELL=%MSYS2_ROOT%\ucrt64.exe"

ECHO Generating MSYS2 environment...

IF NOT EXIST "%MSYS2_INSTALLER%" (
    ECHO Downloading MSYS2 installer...
    powershell -Command "Invoke-WebRequest -Uri '%MSYS2_URL%' -OutFile '%MSYS2_INSTALLER%' -UseBasicParsing"
) ELSE (
    ECHO MSYS2 installer already exists. Skipping download.
)

IF NOT EXIST "%MSYS2_ROOT%" (
    ECHO Installing MSYS2...
    "%MSYS2_INSTALLER%"
    IF ERRORLEVEL 1 (
        ECHO MSYS2 installer failed!
        EXIT /B 1
    )
) ELSE (
    ECHO MSYS2 is already installed. Skipping installation.
)

ECHO Initializing MSYS2 environment...
"%MSYS2_SHELL%" bash -c "pacman -S --noconfirm mingw-w64-ucrt-x86_64-toolchain mingw-w64-ucrt-x86_64-clang mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-vulkan-headers mingw-w64-ucrt-x86_64-vulkan-loader mingw-w64-ucrt-x86_64-vulkan-validation-layers && pacman -Syu --noconfirm"

IF ERRORLEVEL 1 (
    ECHO MSYS2 setup failed!
    EXIT /B 1
)

ECHO MSYS2 environment setup complete.
ENDLOCAL
PAUSE
