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

find_package(PkgConfig REQUIRED)
pkg_check_modules(BSONCXX REQUIRED libbsoncxx)
pkg_check_modules(MONGOCXX REQUIRED libmongocxx)
pkg_check_modules(GUMBO REQUIRED gumbo)
#pkg_check_modules(CURL REQUIRED libcurl)
find_package(CURL REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(OpenSSL REQUIRED)

include_directories(${BSONCXX_INCLUDE_DIRS} ${MONGOCXX_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS} ${GUMBO_LIBRARY_DIRS})
link_directories(${BSONCXX_LIBRARY_DIRS} ${MONGOCXX_LIBRARY_DIRS} ${GUMBO_LIBRARY_DIRS})

# Add the include directories
include_directories("/usr/local/include")
include_directories(modules/h src)

# Add source files # ✅ Search for .cpp files
file(GLOB_RECURSE MODULE_SOURCES_A modules/module_a/*.cpp modules/module_a/*.hpp)
file(GLOB_RECURSE MODULE_SOURCES_B modules/module_b/*.cpp modules/module_b/*.hpp)
file(GLOB_RECURSE MODULE_SOURCES_C modules/module_c/*.cpp modules/module_c/*.hpp)
file(GLOB_RECURSE MODULE_SOURCES_D modules/module_d/*.cpp modules/module_d/*.hpp)
# file(GLOB_RECURSE MODULE_SOURCES modules/src/*.cpp src/*.cpp)  
file(GLOB_RECURSE MODULE_HEADERS modules/h/*.h modules/h/*.hpp src/*.h src/*.hpp)  # ✅ Search for headers

# Print debugging information
# message(STATUS "Sources: ${MODULE_SOURCES}")
# message(STATUS "Found source files: ${MODULE_SOURCES_A}")
 #LmVniuYFmyMpVbGF

# Set the sources for the executable
set(SOURCES 
${MODULE_SOURCES_A} 
${MODULE_SOURCES_B}
${MODULE_SOURCES_C} 
${MODULE_SOURCES_D} 
${MODULE_HEADERS} 
main.cpp)

# Add executable and link C and C++ sources
add_executable(Dunna ${SOURCES})

# Explicitly link the libraries
target_link_libraries(Dunna 
    ${BSONCXX_LIBRARIES} 
    ${MONGOCXX_LIBRARIES} 
    ${CURL_LIBRARIES} 
    ${GUMBO_LIBRARIES}
    nlohmann_json::nlohmann_json
    OpenSSL::SSL 
    OpenSSL::Crypto
    )

# Output binary into build/bin
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})