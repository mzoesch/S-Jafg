@REM BEGIN STUB
@REM Copyright mzoesch. All rights reserved.

setlocal enabledelayedexpansion

set MakeEnvForlatestMsvc_CURRENT_DIR=%cd%
set MSVC_X64_BASE=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\
set MSVC_BASE_DIR=%MSVC_X64_BASE%Tools\MSVC
set MSVC_ENV_CREATE=%MSVC_X64_BASE%Auxiliary\Build
cd /d %MSVC_BASE_DIR%
echo Looking in %MSVC_BASE_DIR% for versions.

set MAX=0
set MAX_FLDR=""
for /d %%F in (*) do (
    set FLDR_NAME=%%~nxF
    for /f "tokens=1 delims=_" %%A in ("!FLDR_NAME!") do (
        set "num=%%A"
        if !num! gtr !MAX! (
            set MAX=!num!
            set MAX_FLDR=%%F
        )
    )
)

set MSVC_PATH=%MSVC_BASE_DIR%\%MAX_FLDR%\bin\Hostx64\x64\cl.exe
if not exist "%MSVC_PATH%" (
    echo MSVC not found! Checked at dir: v[%MAX%] [%MSVC_PATH%].
    exit /b 1
)
echo Using MSVC version [%MAX%] at [%MSVC_PATH%].

set MSVC_INCLUDE_PATH=%MSVC_BASE_DIR%\%MAX_FLDR%\include
if not exist "%MSVC_INCLUDE_PATH%" (
    echo MSVC include dir not found! Checked at dir: [%MSVC_INCLUDE_PATH%].
    exit /b 1
)
echo Using MSVC include path at: [%MSVC_INCLUDE_PATH%].

set ENV_CREATE_FILE=%MSVC_ENV_CREATE%\vcvarsall.bat
if not exist "%ENV_CREATE_FILE%" (
    echo Varsall not found. Checked file: [%ENV_CREATE_FILE%]
    exit /b 1
)
echo Creating environment for this session.
call "%ENV_CREATE_FILE%" x64

@REM END STUB
