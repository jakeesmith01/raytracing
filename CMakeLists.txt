cmake_minimum_required(VERSION 3.10)

# Project name
project(raytracing)

# Set C standard
set(CMAKE_C_STANDARD 99)

# Add executable
add_executable(raytracing src/raytracing.c)

# Find SDL2 package
find_package(SDL2 REQUIRED)

# Include SDL2 directories
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2 and math libraries
target_link_libraries(raytracing ${SDL2_LIBRARIES} m)
