@REM Copyright mzoesch. All rights reserved.

@REM Internal script do not use.
@REM INTERNAl: Call from engine root directory.

@echo off
setlocal

echo "Current directory: %cd%"

if "%~1"=="" (
    echo "No argument given. Please provide a build configuration."
    exit /b 1
)

set arg1=%1
if "%arg1%"=="" (
    echo "No argument given. Please provide a build configuration."
    exit /b 1
)
echo "Build configuration: %arg1%"

set TARGET_DIR=Saved\MinimalWasm-%arg1%

cd /d "%TARGET_DIR%"
if errorlevel 1 (
    echo "Failed to change directory to [%TARGET_DIR%]."
    exit /b 1
)

echo "Build output directory: %cd%"

@REM Make actual build.
cmake --build . --clean-first
