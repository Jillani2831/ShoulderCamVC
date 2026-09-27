@echo off
setlocal
if "%PLUGIN_SDK_DIR%"=="" set "PLUGIN_SDK_DIR=%CD%\plugin-sdk"

if not exist "%PLUGIN_SDK_DIR%\tools\premake\premake5.exe" (
    echo Plugin-SDK Premake not found.
    echo Set PLUGIN_SDK_DIR to your Plugin-SDK folder.
    pause
    exit /b 1
)

"%PLUGIN_SDK_DIR%\tools\premake\premake5.exe" --pluginsdkdir="%PLUGIN_SDK_DIR%" vs2022
pause
