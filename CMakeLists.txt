cmake_minimum_required(VERSION 3.10)
project(mos_skeleton)

set(CMAKE_CXX_STANDARD 14)

# mos
add_subdirectory(externals/mos)
include_directories(externals/mos/include)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} mos)