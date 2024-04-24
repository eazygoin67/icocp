@echo off

rem Setup directory variables
set WORKSPACE_ROOT=%~dp0..\
set PROJDIR=%~dp0..\intermediate\
echo Root Directory: %WORKSPACE_ROOT%
echo Project Directory: %PROJDIR%

rem Create Project Directory if it does not exist
if not exist %PROJDIR% (
    echo The '%PROJDIR%' directory doesn't exist, creating it now...
    mkdir %PROJDIR% 
)

rem Move to Project Directory
pushd %PROJDIR%

rem Find the installed version of CMake
set CMAKE_EXE=cmake
where /q %CMAKE_EXE%
if "%ERRORLEVEL%" NEQ "0" (
    if exist "C:\Program Files\CMake\bin\cmake.exe" (
        set CMAKE_EXE="C:\Program Files\CMake\bin\cmake.exe"
    ) else (
        set CMAKE_EXE=""
    )
)
if %CMAKE_EXE% == "" (
    echo "| ERROR: Couldn't find CMake executable."
    echo "|        Please make sure you have cmake installed, and added to your environment path variable."
    popd
    pause
    exit /B -1
)
echo CMake found at %CMAKE_EXE%

rem Ensure the temp directory for the win32 build exists
if not exist %PROJDIR%\win32 (
    echo The '%PROJDIR%\win32' directory doesn't exist, creating it now...
    mkdir %PROJDIR%\win32
)

rem Generate Win32 version of Project
pushd %PROJDIR%\win32
echo Generating project Win32 Project to '%PROJDIR%'...
%cmake_exe% -G "Visual Studio 17 2022" -A x64 -Thost=x64 -DFPS_PLATFORM="Win32" -Dfreetype=enabled .\..\.. %ARGS%
popd
if "%ERRORLEVEL%" NEQ "0" (
    echo "| ERROR: Failed to generate cmake project"
    popd
    pause
    exit /B -1
)

echo Generation Successful...

popd
pause
exit /B 0
