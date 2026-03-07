#!/bin/bash
# ESP32-S3 Watch Flash Script for Linux/Mac
# Requires: Python + esptool.py (pip install esptool)

echo "========================================"
echo "ESP32-S3 Watch Firmware Flash Tool"
echo "========================================"
echo ""

# Check if port is provided
if [ -z "$1" ]; then
    echo "Usage: ./flash_linux.sh [PORT]"
    echo "Example: ./flash_linux.sh /dev/ttyUSB0"
    echo ""
    echo "Available serial ports:"
    if [ "$(uname)" == "Darwin" ]; then
        ls -l /dev/cu.usbserial* /dev/cu.usbmodem* 2>/dev/null
    else
        ls -l /dev/ttyUSB* /dev/ttyACM* 2>/dev/null
    fi
    echo ""
    read -p "Enter port (e.g., /dev/ttyUSB0): " PORT
else
    PORT=$1
fi

echo ""
echo "Flashing to $PORT..."
echo ""

# Flash all components
esptool.py --chip esp32s3 -p "$PORT" \
    --before default_reset \
    --after hard_reset \
    write_flash \
    --flash_mode dio \
    --flash_freq 80m \
    --flash_size 2MB \
    0x0 bootloader.bin \
    0x8000 partition-table.bin \
    0x10000 esp32_s3_watch.bin

if [ $? -eq 0 ]; then
    echo ""
    echo "========================================"
    echo "Flash SUCCESSFUL!"
    echo "========================================"
    echo ""
    echo "The device will restart automatically."
    echo ""
else
    echo ""
    echo "========================================"
    echo "Flash FAILED!"
    echo "========================================"
    echo ""
    echo "Troubleshooting:"
    echo "1. Check USB cable connection"
    echo "2. Hold BOOT button while connecting USB"
    echo "3. Try a different USB port"
    echo "4. Install CP210x/CH340 drivers if needed"
    echo "5. Run with sudo if permission denied"
    echo ""
fi
