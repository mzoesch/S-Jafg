@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will install all the necessary dependencies for this project
@REM and generate the necessary platfrom dependent solution files.

echo Calling Python to generate solution files ...

python %~dp0/Program.py --install-deps --compile-scripts --generate-solution --emulate

pause
