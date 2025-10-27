cmake_minimum_required(VERSION 3.15)
project(MEVShield)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Compiler optimizations
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2 -Wall -Wextra")

# Include directories
include_directories(
    src
    src/common
    src/network
    src/analytics
    src/core
)

# Source files
set(SOURCES
    src/main.cpp
    src/common/config_loader.cpp
)

# Create executable
add_executable(mev_shield ${SOURCES})

# Link libraries (Ubuntu package names)
target_link_libraries(mev_shield
    pthread
    fmt
    spdlog
    yaml-cpp
    jsoncpp
    boost_system
    boost_thread
    ssl
    crypto
    curl
)

# Compiler definitions
target_compile_definitions(mev_shield PRIVATE SPDLOG_FMT_EXTERNAL)
