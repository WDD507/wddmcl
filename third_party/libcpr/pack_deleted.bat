@echo off

mkdir bin 2>nul
mkdir examples 2>nul

move /Y libcurl-x64.dll bin\ 2>nul
move /Y test_cpr.cpp examples\ 2>nul

del /Q test_cpr.exe 2>nul

echo Pack complete!
dir
