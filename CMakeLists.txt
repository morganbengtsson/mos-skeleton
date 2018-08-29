cmake_minimum_required(VERSION 3.1.0)
project(mos_skeleton)

set(CMAKE_CXX_STANDARD 17)

# mos
add_subdirectory(externals/mos)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC mos)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
