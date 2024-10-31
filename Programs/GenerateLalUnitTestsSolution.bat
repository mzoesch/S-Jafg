@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will bring the solution to only have the lal environment and the unit tests for it.
@REM This significantly reduces compile times when making changes to critical parts of this app.

pushd ..

echo Calling Python to generate solution files ...

python Program.py --generate-solution-for-lal-unit-tests

popd

pause
