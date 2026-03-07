@echo off
echo ========================================
echo ESP32-S3 Watch Firmware Flash Tool
echo ========================================
echo.
if "%1"=="" (
    echo Usage: flash_windows.bat [COM_PORT]
    echo Example: flash_windows.bat COM8
    echo.
    set /p COM_PORT="Enter COM port (e.g., COM8): "
) else (
    set COM_PORT=%1
)
echo.
echo Flashing to %COM_PORT%...
echo.
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
) else (
    echo.
    echo ========================================
    echo Flash FAILED!
    echo ========================================
)
pause
