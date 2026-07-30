@echo off
rem Build FuCad on Windows without the pixi launcher.
rem Activates MSVC, then puts the pixi conda environment on PATH so the Qt
rem tools (rcc, uic, moc) can resolve their DLLs from Library\bin.
rem Usage: tools\build-win.bat [ninja target ...]

setlocal

set "REPO=%~dp0.."
set "ENVDIR=%REPO%\.pixi\envs\default"
set "BUILDDIR=%REPO%\build\release"

if not exist "%ENVDIR%" (
    echo ERROR: pixi environment not found at "%ENVDIR%"
    exit /b 1
)

set "VCVARS=C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
if not exist "%VCVARS%" (
    echo ERROR: vcvars64.bat not found at "%VCVARS%"
    exit /b 1
)
call "%VCVARS%" >nul 2>&1
if errorlevel 1 (
    echo ERROR: failed to initialize the MSVC environment
    exit /b 1
)

set "PATH=%ENVDIR%;%ENVDIR%\Library\mingw-w64\bin;%ENVDIR%\Library\usr\bin;%ENVDIR%\Library\bin;%ENVDIR%\Scripts;%ENVDIR%\bin;%PATH%"

cmake --build "%BUILDDIR%" %*
exit /b %errorlevel%
