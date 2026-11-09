cmake_minimum_required(VERSION 3.10)
project(Demuxer)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add your source files here
add_executable(demuxer
    # Add your .cpp files
    firmware/usb_output.c  # Include the new USB output source file
)