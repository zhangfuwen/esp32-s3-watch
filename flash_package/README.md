# ESP32-S3 Watch Firmware Flash Package

## Contents
- `bootloader.bin` - Flash to 0x0
- `partition-table.bin` - Flash to 0x8000
- `esp32_s3_watch.bin` - Flash to 0x10000
- `flash_windows.bat` - Windows flash script
- `flash_linux.sh` - Linux/Mac flash script

## Prerequisites
Install esptool: `pip install esptool`

## Flash Instructions
**Windows:** Double-click `flash_windows.bat` or run `flash_windows.bat COM8`

**Linux/Mac:** `./flash_linux.sh /dev/ttyUSB0`

## Manual Flash
```bash
esptool.py --chip esp32s3 -p [PORT] write_flash \
    --flash_mode dio --flash_freq 80m --flash_size 2MB \
    0x0 bootloader.bin \
    0x8000 partition-table.bin \
    0x10000 esp32_s3_watch.bin
```

## Version
Built from source. Check git log for details.
