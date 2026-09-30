cmake_minimum_required(VERSION 3.10)
project(Demuxer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(firmware)

# Add source files
add_executable(demuxer
    firmware/main.c
    firmware/demuxer.h
    firmware/expansion_modules.c
    firmware/flex.c
    firmware/foil.c
    firmware/haptic.c
    firmware/imu.c
    firmware/touchscreen.c
    firmware/wifi_brick.c
)

# Optional: Add tests if you have them
enable_testing()
# add_test(...)
