@echo off
setlocal

if not exist build mkdir build

cl .\bin\build.cpp /Febuild\game.exe /Fobuild\ /Fdbuild\game.pdb /Zi /Od /permissive- /W4 /EHsc shell32.lib
del /Q build\*.obj >nul 2>nul