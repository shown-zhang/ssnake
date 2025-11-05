@echo off
echo Cleaning up...
rd /s /q build >nul 2>&1
rd /s /q bin >nul 2>&1
echo.
echo Building...
md build && cd build
cmake -G "MinGW Makefiles" ..
cmake --build .
cd ..
echo Build successful!