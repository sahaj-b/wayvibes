cmake_minimum_required(VERSION 3.10)

project(wayvibes)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Specify the source files
set(SOURCES
    src/main.cpp
    src/audio.cpp
    src/device.cpp
    src/config.cpp
)

# Include directories
include_directories(src)

# Add the executable
add_executable(wayvibes ${SOURCES})

# Link libraries
target_link_libraries(wayvibes evdev)
