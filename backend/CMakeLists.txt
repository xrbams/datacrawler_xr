cmake_minimum_required(VERSION 3.14)
project(Dunna LANGUAGES C CXX)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Specify the build type (Debug/Release)
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Add the include directories
include_directories(modules/h src)

# Add source files
file(GLOB_RECURSE MODULE_SOURCES modules/src/*.cpp src/*.cpp)  # ✅ Search for .cpp files
file(GLOB_RECURSE MODULE_HEADERS modules/h/*.h modules/h/*.hpp src/*.h src/*.hpp)  # ✅ Search for headers

# Print debugging information to ensure files are found
# message(STATUS "Found source files: ${MODULE_SOURCES}")
# message(STATUS "Found header files: ${MODULE_HEADERS}")

# Set the sources for the executable
set(SOURCES ${MODULE_SOURCES} ${MODULE_HEADERS} main.cpp)

# Add executable and link C and C++ sources
add_executable(Dunna ${SOURCES})

# Output binary into build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})