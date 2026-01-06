@echo off
set PATH=.;%PATH%

powershell -ExecutionPolicy Bypass -Command "& { Import-Module 'C:\\Program Files\\Microsoft Visual Studio\\18\\Community\\Common7\\Tools\\Microsoft.VisualStudio.DevShell.dll'; Enter-VsDevShell -Arch amd64 -SkipAutomaticLocation %* 1bc2c467; .\build.ps1}"
pause
