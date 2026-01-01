@echo off
echo Building Jalan Engine with Ninja (Debug)...

REM Create build directory if it doesn't exist
if not exist "build" mkdir build

REM Navigate to build directory
cd build

REM Configure with CMake and Ninja for debug
cmake -G Ninja -DCMAKE_BUILD_TYPE=Debug ..

REM Build the project
ninja

REM Return to root directory
cd ..

echo.
echo Debug build complete! Executables are in the build directory:
echo - jalan_editor.exe (Editor)
echo - jalanallas.exe (Game)
echo.
pause