@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Development only script.

pushd ..

.\.venv\Scripts\python.exe .\Program.py -INVOKE -- --DoNothing

popd

pause
