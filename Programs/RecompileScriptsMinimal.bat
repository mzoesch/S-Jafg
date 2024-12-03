@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will install all the necessary dependencies for this project.

pushd ..

echo Calling Python to generate solution files ...

python Program.py --compile-scripts

popd

pause
