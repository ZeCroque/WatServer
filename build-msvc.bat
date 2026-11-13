@echo off
setlocal

pushd "%~dp0"
if errorlevel 1 exit /b 1

where cl >nul 2>nul
if errorlevel 1 (
    echo MSVC compiler not found. Run this script from a Visual Studio Developer Command Prompt or Developer PowerShell.
    popd
    exit /b 1
)

if not exist build mkdir build
if errorlevel 1 (
    echo Could not create the build directory.
    popd
    exit /b 1
)

cl /nologo /EHsc /std:c++14 /Fobuild\ /Febuild\server.exe server.cpp common.cpp /link Ws2_32.lib
set "build_result=%errorlevel%"

popd
exit /b %build_result%
