#!/bin/bash
echo "========================================"
echo "ESP32-S3 Watch Firmware Flash Tool"
echo "========================================"
echo ""
if [ -z "$1" ]; then
    echo "Usage: ./flash_linux.sh [PORT]"
    echo "Example: ./flash_linux.sh /dev/ttyUSB0"
    echo ""
    read -p "Enter port (e.g., /dev/ttyUSB0): " PORT
else
    PORT=$1
fi
echo ""
echo "Flashing to $PORT..."
echo ""
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
else
    echo ""
    echo "========================================"
    echo "Flash FAILED!"
    echo "========================================"
fi
