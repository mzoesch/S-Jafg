@echo off

pushd ..
Vendor\Premake\Win\premake5.exe --file=Build.lua vs2022
popd
pause
