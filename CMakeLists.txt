cmake_minimum_required(VERSION 3.10)

project(auv-host)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(vendor/sol2)
set(SOL2_LUA_VERSION "LuaJIT")
set(SOL2_BUILD_LUA false)

add_subdirectory(runtime)
add_subdirectory(vision)
add_subdirectory(application)

set(EXECUTABLE_OUTPUT_PATH .)
set(LIBRARY_OUTPUT_PATH libs)
