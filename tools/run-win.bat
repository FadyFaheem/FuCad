@echo off
rem Run FuCad from the build tree on Windows.
rem Puts the pixi conda environment on PATH and points Qt at its plugin
rem directory, which the build output does not carry a qt.conf for.
rem Usage: tools\run-win.bat [FreeCAD arguments ...]

setlocal

set "REPO=%~dp0.."
set "ENVDIR=%REPO%\.pixi\envs\default"
set "EXE=%REPO%\build\release\bin\FreeCAD.exe"

if not exist "%EXE%" (
    echo ERROR: "%EXE%" not found. Run tools\build-win.bat first.
    exit /b 1
)

set "PATH=%ENVDIR%;%ENVDIR%\Library\mingw-w64\bin;%ENVDIR%\Library\usr\bin;%ENVDIR%\Library\bin;%ENVDIR%\Scripts;%ENVDIR%\bin;%PATH%"
set "QT_PLUGIN_PATH=%ENVDIR%\Library\lib\qt6\plugins"

start "" "%EXE%" %*
