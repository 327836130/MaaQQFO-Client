@echo off
setlocal
if /i "%~1"=="x86" (set "arch=x86") else if /i "%~1"=="x64" (set "arch=x64") else exit /b 2
if not defined QQFO_VS_INSTALL for /f "usebackq delims=" %%I in (`"%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe" -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath`) do set "QQFO_VS_INSTALL=%%I"
if not defined QQFO_VS_INSTALL exit /b 3
call "%QQFO_VS_INSTALL%\VC\Auxiliary\Build\vcvarsall.bat" %arch% >nul
if errorlevel 1 exit /b 1
cd /d "%~dp0.."
if not exist artifacts\a3-native mkdir artifacts\a3-native
cl /nologo /W4 /WX /std:c++17 /EHsc QQFO\Controller\Native\InputProtocolSmoke.cpp /Foartifacts\a3-native\%arch%.obj /Feartifacts\a3-native\%arch%.exe /link Advapi32.lib User32.lib
if errorlevel 1 exit /b 1
artifacts\a3-native\%arch%.exe
exit /b %errorlevel%
