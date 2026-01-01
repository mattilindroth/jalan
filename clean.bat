@echo off
echo Cleaning build directory...

REM Remove build directory if it exists
if exist "build" (
    rmdir /s /q "build"
    echo Build directory cleaned.
) else (
    echo Build directory doesn't exist.
)

echo.
pause