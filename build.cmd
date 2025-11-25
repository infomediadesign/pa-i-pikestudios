@echo off
set PATH=.;%PATH%

powershell -ExecutionPolicy Bypass -File .\build.ps1 %*
pause
