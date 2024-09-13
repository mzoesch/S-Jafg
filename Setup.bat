@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will install all the necessary dependencies for this project
@REM and generate the necessary platfrom dependent solution files.

echo Calling Python to generate solution files ...

python Program.py run=BuildTool --install-deps --generate-solution

pause
