# ESP32-S3 Watch Project - CMake Build Configuration
cmake_minimum_required(VERSION 3.16)

# Include ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Project name
project(esp32_s3_watch)

# Set target chip
set(IDF_TARGET "esp32s3")

# Component dependencies
set(EXTRA_COMPONENT_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/lib
)

# Build configuration
idf_component_register(
    SRCS 
        "src/main.c"
        "src/display.c"
        "src/watch_face.c"
        "src/input.c"
        "src/wifi.c"
        "src/bluetooth.c"
    INCLUDE_DIRS 
        "include"
    REQUIRES
        driver
        esp_wifi
        esp_bt
        nvs_flash
        lwip
)

# Compiler options
target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wall
    -Wextra
    -Werror
)

# Flash configuration
esptool_py_create_target_image()
