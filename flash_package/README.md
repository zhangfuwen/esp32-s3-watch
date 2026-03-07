# ESP32-S3 Watch Firmware Flash Package

Version: v0.2.0  
Build Date: 2026-03-07  
Firmware: ESP32-S3 Watch with Emotional Comfort Features

## Files Included

- `bootloader.bin` - ESP32-S3 bootloader (flashes to 0x0)
- `partition-table.bin` - Partition table (flashes to 0x8000)
- `esp32_s3_watch.bin` - Main application (flashes to 0x10000)
- `flash_windows.bat` - Windows flash script
- `flash_linux.sh` - Linux/Mac flash script

## Prerequisites

**Python 3.x** and **esptool.py** required:
```bash
pip install esptool
```

## Flash Instructions

### Windows
```cmd
flash_windows.bat COM8
```
Or double-click `flash_windows.bat` and follow prompts.

### Linux/Mac
```bash
chmod +x flash_linux.sh
./flash_linux.sh /dev/ttyUSB0
```

### Manual Flash (Any Platform)
```bash
esptool.py --chip esp32s3 -p [PORT] write_flash \
    --flash_mode dio \
    --flash_freq 80m \
    --flash_size 2MB \
    0x0 bootloader.bin \
    0x8000 partition-table.bin \
    0x10000 esp32_s3_watch.bin
```

## Memory Map

| Address   | Component            | Size    |
|-----------|---------------------|---------|
| 0x0000    | Bootloader          | ~30KB   |
| 0x8000    | Partition Table     | ~4KB    |
| 0x10000   | Application         | ~300KB  |

## Features (v0.2.0)

### Emotional Comfort Features
- **Breathing Companion** - 4-7-8, box, and coherent breathing patterns
- **Comfort Messages** - 40+ encouraging messages throughout the day
- **Emergency Calm Button** - Press both buttons for instant calm support

### Hardware Support
- ESP32-S3 (espwatch-s3b2 board)
- SPI display (240x284)
- BOOT button (GPIO 0)
- POWER button (GPIO 45)
- Backlight (GPIO 47)
- Audio codec ES8311 (stub)
- Vibration motor (GPIO 4, stub)

## Troubleshooting

### Flash Fails
1. **Hold BOOT button** while connecting USB, then release
2. Try different USB port/cable
3. Install CP210x or CH340 USB drivers
4. Run as administrator (Windows) or with sudo (Linux)

### Device Not Detected
- Windows: Check Device Manager → Ports (COM & LPT)
- Linux: `ls /dev/ttyUSB*` or `dmesg | grep tty`
- Mac: `ls /dev/cu.*`

### Watch Not Booting
1. Erase flash: `esptool.py -p [PORT] erase_flash`
2. Re-flash using script
3. Check power supply (USB cable)

## GitHub Repository

https://github.com/zhangfuwen/esp32-s3-watch

## License

MIT License - See LICENSE file in main repository
