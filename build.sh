#!/bin/bash
# ESP32-S3 Watch Firmware Build Script
# 
# This script builds the ESP32-S3 Watch firmware using the locally installed ESP-IDF.
# It creates a flash-ready package with all necessary binaries and scripts.
#
# Usage: ./build.sh [clean]
#   clean - Optional: Clean build directory before building

set -e  # Exit on error

#==================== Configuration ====================
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$SCRIPT_DIR"
BUILD_DIR="$PROJECT_DIR/build"
FLASH_PACKAGE_DIR="$PROJECT_DIR/flash_package"
OUTPUT_DIR="$PROJECT_DIR/dist"

# ESP-IDF installation path
export IDF_PATH="/home/admin/esp-idf"
export IDF_TOOLS_PATH="/home/admin/.espressif"

#==================== Colors ====================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

#==================== Functions ====================
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warn() {
    echo -e "${YELLOW}[WARN]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_env() {
    log_info "Checking ESP-IDF environment..."
    
    if [ ! -d "$IDF_PATH" ]; then
        log_error "ESP-IDF not found at $IDF_PATH"
        log_error "Please install ESP-IDF or update IDF_PATH in this script"
        exit 1
    fi
    
    if [ ! -f "$IDF_PATH/export.sh" ]; then
        log_error "export.sh not found at $IDF_PATH/export.sh"
        exit 1
    fi
    
    # Source ESP-IDF environment
    source "$IDF_PATH/export.sh"
    
    log_success "ESP-IDF environment loaded: $(idf.py --version)"
}

clean_build() {
    log_info "Cleaning build directory..."
    rm -rf "$BUILD_DIR"
    rm -rf "$FLASH_PACKAGE_DIR"
    rm -rf "$OUTPUT_DIR"
    log_success "Clean complete"
}

build_firmware() {
    log_info "Building ESP32-S3 firmware..."
    cd "$PROJECT_DIR"
    
    # Set target and build
    idf.py set-target esp32s3
    idf.py build
    
    if [ $? -eq 0 ]; then
        log_success "Build completed successfully!"
    else
        log_error "Build failed!"
        exit 1
    fi
}

create_flash_package() {
    log_info "Creating flash package..."
    
    # Create directories
    mkdir -p "$FLASH_PACKAGE_DIR"
    mkdir -p "$OUTPUT_DIR"
    
    # Copy binaries
    cp "$BUILD_DIR/bootloader/bootloader.bin" "$FLASH_PACKAGE_DIR/"
    cp "$BUILD_DIR/partition_table/partition-table.bin" "$FLASH_PACKAGE_DIR/"
    cp "$BUILD_DIR/esp32_s3_watch.bin" "$FLASH_PACKAGE_DIR/"
    cp "$BUILD_DIR/flash_args" "$FLASH_PACKAGE_DIR/"
    
    # Create Windows flash script
    cat > "$FLASH_PACKAGE_DIR/flash_windows.bat" << 'SCRIPT'
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
SCRIPT

    # Create Linux/Mac flash script
    cat > "$FLASH_PACKAGE_DIR/flash_linux.sh" << 'SCRIPT'
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
SCRIPT
    chmod +x "$FLASH_PACKAGE_DIR/flash_linux.sh"
    
    # Create README
    cat > "$FLASH_PACKAGE_DIR/README.md" << 'README'
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
README

    log_success "Flash package created at $FLASH_PACKAGE_DIR"
}

create_archive() {
    log_info "Creating distribution archive..."
    
    # Get version from git
    VERSION=$(git describe --tags --always --dirty 2>/dev/null || echo "dev")
    DATE=$(date +%Y%m%d)
    ARCHIVE_NAME="esp32-s3-watch-${VERSION}-${DATE}-flash-package.zip"
    
    # Create ZIP
    cd "$FLASH_PACKAGE_DIR"
    zip -r "$OUTPUT_DIR/$ARCHIVE_NAME" .
    
    log_success "Archive created: $OUTPUT_DIR/$ARCHIVE_NAME"
    
    # List contents
    echo ""
    log_info "Archive contents:"
    ls -lh "$OUTPUT_DIR/$ARCHIVE_NAME"
}

print_summary() {
    echo ""
    echo "========================================"
    echo "        BUILD COMPLETE!"
    echo "========================================"
    echo ""
    log_info "Flash package: $FLASH_PACKAGE_DIR"
    log_info "Archive: $OUTPUT_DIR/"
    echo ""
    log_info "To flash:"
    echo "  Windows: cd flash_package && flash_windows.bat COM8"
    echo "  Linux:   cd flash_package && ./flash_linux.sh /dev/ttyUSB0"
    echo ""
    log_success "Done!"
}

#==================== Main ====================
main() {
    echo "========================================"
    echo "ESP32-S3 Watch Firmware Builder"
    echo "========================================"
    echo ""
    
    # Check for clean flag
    if [ "$1" = "clean" ]; then
        clean_build
    fi
    
    # Check environment
    check_env
    
    # Build
    build_firmware
    
    # Create flash package
    create_flash_package
    
    # Create archive
    create_archive
    
    # Summary
    print_summary
}

# Run main
main "$@"
