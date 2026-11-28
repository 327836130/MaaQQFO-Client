@echo off
setlocal
if not defined QQFO_VS_INSTALL for /f "usebackq delims=" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "QQFO_VS_INSTALL=%%I"
if not defined QQFO_VS_INSTALL exit /b 3
call "%QQFO_VS_INSTALL%\VC\Auxiliary\Build\vcvarsall.bat" x64 >nul
if errorlevel 1 exit /b 1
cd /d "%~dp0.."
if not exist artifacts\a6-state mkdir artifacts\a6-state
cl /nologo /W4 /WX /std:c++17 /EHsc QQFO\GameState\GameStateSmoke.cpp /Foartifacts\a6-state\state.obj /Feartifacts\a6-state\state.exe
if errorlevel 1 exit /b 1
artifacts\a6-state\state.exe %*
exit /b %errorlevel%
