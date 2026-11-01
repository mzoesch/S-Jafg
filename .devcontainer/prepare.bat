@echo off
setlocal

set "WORKSPACE_ROOT=%~dp0.."
for %%I in ("%WORKSPACE_ROOT%") do set "WORKSPACE_ROOT=%%~fI"

if not exist "%WORKSPACE_ROOT%\Temp\" (
    mkdir "%WORKSPACE_ROOT%\Temp"
    echo "%WORKSPACE_ROOT%\Temp"
    )

if not exist "%WORKSPACE_ROOT%\Temp\.ccache\" (
    mkdir "%WORKSPACE_ROOT%\Temp\.ccache"
    echo "%WORKSPACE_ROOT%\Temp\.ccache"
    )

endlocal
