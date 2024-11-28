@REM Copyright mzoesch. All rights reserved.

@REM Shortcut script to quickly compile jafg for the wasm platform in debug mode as client.

@echo off

pushd ..
pushd ..

echo Starting bash script in directory: [%cd%].
python ./Program.py --custom-command --WasmRuntime --BUILD_CONFIG=Debug-Client --PLATFORM=Wasm --clean-first-cmake

popd
popd

pause
