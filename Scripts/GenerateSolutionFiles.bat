@echo off

pushd ..
echo Calling Python to generate solution files.
python Build.py
popd

pause
