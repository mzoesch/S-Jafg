@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will install all the necessary dependencies for this project
@REM and generate the necessary platform dependent solution files.

pushd ..

echo Calling Python to generate solution files ...

python Program.py --compile-scripts --generate-solution

popd

pause
