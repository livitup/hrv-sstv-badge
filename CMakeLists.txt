cmake_minimum_required(VERSION 3.13)

# FORCE cross-compilation mode - must be before project()
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_CROSSCOMPILING TRUE)

# Explicitly set the toolchain
set(CMAKE_C_COMPILER /opt/homebrew/bin/arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER /opt/homebrew/bin/arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER /opt/homebrew/bin/arm-none-eabi-gcc)

# Don't try to run test programs (they can't run on host)
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)

# Prevent CMake from adding macOS-specific flags
set(CMAKE_OSX_DEPLOYMENT_TARGET "")
set(CMAKE_OSX_SYSROOT "")

# Target RP2350 board
set(PICO_BOARD pico2)

# Include Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Now define the project
project(defcon_sstv_badge C CXX ASM)

# Set C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Initialize Pico SDK
pico_sdk_init()

# Create main executable
add_executable(defcon_sstv_badge
        src/main.c
)

# Link required libraries
target_link_libraries(defcon_sstv_badge
        pico_stdlib
        hardware_gpio
)

# Generate UF2 file for easy flashing
pico_add_extra_outputs(defcon_sstv_badge)

# Enable USB serial for debug output
pico_enable_stdio_usb(defcon_sstv_badge 1)
pico_enable_stdio_uart(defcon_sstv_badge 0)