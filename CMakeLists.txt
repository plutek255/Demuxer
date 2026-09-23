cmake_minimum_required(VERSION 3.10)
project(Demuxer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files here
add_executable(demuxer
    firmware/main.c
    firmware/demuxer.h
    firmware/flex.c
    firmware/foil.c
    firmware/haptic.c
    firmware/imu.c
    firmware/touchscreen.c
    firmware/wifi_brick.c
)

# Optional: Add tests if you have them
enable_testing()
add_executable(test_demuxer
    tests/test_main.c
)

add_test(NAME TestDemuxer COMMAND test_demuxer)
