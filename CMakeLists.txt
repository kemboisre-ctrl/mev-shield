cmake_minimum_required(VERSION 3.15)
project(MEVShield)

set(CMAKE_CXX_STANDARD 17)
add_executable(mev_shield src/main.cpp)
target_include_directories(mev_shield PRIVATE src)
target_link_libraries(mev_shield PRIVATE spdlog boost_system boost_thread curl ssl crypto pthread stdc++fs)
