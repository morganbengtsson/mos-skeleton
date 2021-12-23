cmake_minimum_required(VERSION 3.1.0)
project(mos_skeleton)

include(FetchContent)

set(CMAKE_CXX_STANDARD 20)

# mos
FetchContent_Declare(
  mos
  GIT_REPOSITORY "https://github.com/morganbengtsson/mos.git"
)
FetchContent_MakeAvailable(mos)

add_executable(${PROJECT_NAME} main.cpp)
target_link_libraries(${PROJECT_NAME} PUBLIC mos)

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})
