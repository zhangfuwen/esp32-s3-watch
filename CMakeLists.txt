# ESP32-S3 Watch Project - CMake Build Configuration
# Top-level CMakeLists.txt for ESP-IDF v5.2

cmake_minimum_required(VERSION 3.16)

# Include ESP-IDF build system
include($ENV{IDF_PATH}/tools/cmake/project.cmake)

# Project name
project(esp32_s3_watch)

# Set target chip
set(IDF_TARGET "esp32s3")

# Build configuration options
idf_build_set_property(COMPILE_OPTIONS "-Wall" APPEND)
idf_build_set_property(COMPILE_OPTIONS "-Wextra" APPEND)
