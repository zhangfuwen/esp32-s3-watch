# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/admin/esp-idf/components/bootloader/subproject"
  "/home/admin/Code/esp32-s3-watch/build/bootloader"
  "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix"
  "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/tmp"
  "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/src/bootloader-stamp"
  "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/src"
  "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/admin/Code/esp32-s3-watch/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
