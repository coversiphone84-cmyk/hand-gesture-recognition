cmake_minimum_required(VERSION 3.10)
project(gesture_recognition)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenCV
find_package(OpenCV 4.5 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

# Find JsonCpp
find_package(jsoncpp REQUIRED)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)

# Source files
set(SOURCES
    src/main.cpp
    src/gesture_detector.cpp
    src/socket_client.cpp
)

# Create executable
add_executable(gesture_recognition ${SOURCES})

# Link libraries
target_link_libraries(gesture_recognition
    ${OpenCV_LIBS}
    jsoncpp
    pthread
)

# Compiler flags
if(MSVC)
    target_compile_options(gesture_recognition PRIVATE /W4)
else()
    target_compile_options(gesture_recognition PRIVATE -Wall -Wextra -Wpedantic)
endif()

print_status("Build configuration:")
print_status("  OpenCV: ${OpenCV_VERSION}")
print_status("  C++ Standard: ${CMAKE_CXX_STANDARD}")
