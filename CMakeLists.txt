cmake_minimum_required(VERSION 3.10)

project(auv-host)

set(CMAKE_CXX_STANDARD 17)
set(SOL2_LUA_VERSION "LuaJIT")
set(SOL2_BUILD_LUA false)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

find_package(LuaJit REQUIRED)
find_package(OpenCV REQUIRED)

include_directories(${LUAJIT_INCLUDE_DIRS})
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(runtime)

add_subdirectory(communication)
add_subdirectory(vision)
add_subdirectory(runtime)
add_subdirectory(application)
