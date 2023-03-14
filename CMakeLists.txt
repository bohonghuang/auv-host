cmake_minimum_required(VERSION 3.10)

project(auv-host)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(vendor/sol2)
set(SOL2_LUA_VERSION "LuaJIT")
set(SOL2_BUILD_LUA TRUE)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(runtime)
add_subdirectory(vision)
add_subdirectory(application)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/.)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
