cmake_minimum_required(VERSION 3.10)
project(Demuxer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add source files here
add_executable(demuxer
    firmware/main.c
    # Add other necessary source files if any
)

# Include directories if needed
include_directories(firmware)

# Optional: Add libraries if needed
# target_link_libraries(demuxer <library-name>)

# Enable testing if needed
enable_testing()
# add_test(<test-name> <executable>)
