# Minimum CMake version
cmake_minimum_required(VERSION 3.10)

# Project name and version
project(Zettabolt VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specify the output directory for the executable (set to the working directory).
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR})

# Include the directory containing header files.
include_directories(${PROJECT_SOURCE_DIR}/include)

# Collect all source files.
set(SOURCES
    src/main.cpp
    src/data_loader.cpp
    src/data_manager.cpp
)

# Create the executable.
add_executable(Zettabolt ${SOURCES})

# Link pthread library
find_package(Threads REQUIRED)
target_link_libraries(Zettabolt PRIVATE Threads::Threads)