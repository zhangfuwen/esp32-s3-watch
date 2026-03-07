@echo off
REM ESP32-S3 Watch Flash Script for Windows
REM Requires: Python + esptool.py (pip install esptool)

echo ========================================
echo ESP32-S3 Watch Firmware Flash Tool
echo ========================================
echo.

REM Check if COM port is provided
if "%1"=="" (
    echo Usage: flash_windows.bat [COM_PORT]
    echo Example: flash_windows.bat COM8
    echo.
    echo Available COM ports:
    mode
    echo.
    set /p COM_PORT="Enter COM port (e.g., COM8): "
) else (
    set COM_PORT=%1
)

echo.
echo Flashing to %COM_PORT%...
echo.

REM Flash all components
esptool.py --chip esp32s3 -p %COM_PORT% ^
    --before default_reset ^
    --after hard_reset ^
    write_flash ^
    --flash_mode dio ^
    --flash_freq 80m ^
    --flash_size 2MB ^
    0x0 bootloader.bin ^
    0x8000 partition-table.bin ^
    0x10000 esp32_s3_watch.bin

if %ERRORLEVEL% EQU 0 (
    echo.
    echo ========================================
    echo Flash SUCCESSFUL!
    echo ========================================
    echo.
    echo The device will restart automatically.
    echo.
) else (
    echo.
    echo ========================================
    echo Flash FAILED!
    echo ========================================
    echo.
    echo Troubleshooting:
    echo 1. Check USB cable connection
    echo 2. Hold BOOT button while connecting USB
    echo 3. Try a different USB port
    echo 4. Install CP210x/CH340 drivers if needed
    echo.
)

echo Press any key to exit...
pause >nul
