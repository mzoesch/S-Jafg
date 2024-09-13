@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will only regenerate the solution files for the project
@REM and will not check for deps if they are installed or not.
@REM Will fail if the dependencies are not installed.

echo Calling Python to generate solution files ...

python Program.py run=BuildTool --generate-solution

pause
