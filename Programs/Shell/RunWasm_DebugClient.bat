@REM Copyright mzoesch. All rights reserved.

@REM Shortcut to quickly server the compiled .html file over a browser. 

@echo off

pushd ..
pushd ..

echo Current directory: %cd%

set TARGET_DIR=Binaries\Wasm-wasm32\Debug-Client\Runtime\

cd /d "%TARGET_DIR%"
if errorlevel 1 (
    echo "Failed to change directory to [%TARGET_DIR%]."
    popd
    popd
    pause
    exit /b 1
)

echo Launch directory: %cd%

emrun Runtime.html

popd
popd
