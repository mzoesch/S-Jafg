@REM Copyright mzoesch. All rights reserved.

@echo off

@REM Will recompile the scripts for this program.

pushd ..

echo Calling Python to generate solution files ...

.\.venv\Scripts\python.exe .\Program.py --ValidateGo --UpdateCachedData --INVOKE --SolutionGenerator GenerateAll

popd

pause
