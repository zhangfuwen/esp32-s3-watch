# ESP32-S3 Watch Project 🎯

A smart watch firmware project for ESP32-S3 development board.

## Hardware

- **MCU**: ESP32-S3 (Dual-core Xtensa LX7, up to 240 MHz)
- **Wireless**: WiFi 802.11 b/g/n, Bluetooth 5 (LE)
- **Display**: TBD (likely SPI/I2C OLED or LCD)
- **Sensors**: TBD (accelerometer, heart rate, etc.)
- **Battery**: LiPo with charging circuit

## Development Environment

### Prerequisites

```bash
# Install ESP-IDF (Espressif IoT Development Framework)
git clone -b v5.2 https://github.com/espressif/esp-idf.git
cd esp-idf
./install.sh esp32s3
source export.sh
```

### Build & Flash

```bash
# Build project
idf.py build

# Flash to device
idf.py -p /dev/ttyUSB0 flash

# Monitor serial output
idf.py -p /dev/ttyUSB0 monitor
```

## Project Structure

```
esp32-s3-watch/
├── src/           # Main source files
├── include/       # Header files
├── lib/           # External libraries
├── tests/         # Unit tests
├── docs/          # Documentation
├── CMakeLists.txt # Build configuration
└── README.md      # This file
```

## Features (Planned)

- [ ] Basic watch face (time/date display)
- [ ] Touch/button input handling
- [ ] WiFi connectivity
- [ ] Bluetooth LE pairing
- [ ] Notification system
- [ ] Health tracking (steps, heart rate)
- [ ] Power management

## Status

🟡 **Initial Setup** - Project created, awaiting hardware specifications

## Next Steps

1. Confirm hardware specifications (display type, sensors, etc.)
2. Set up ESP-IDF development environment
3. Create basic "Hello World" firmware
4. Implement display driver
5. Build watch face UI

---

**Created**: 2026-03-06  
**Developer**: User + AI Assistant
