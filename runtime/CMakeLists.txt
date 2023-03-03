cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "../cmake/")

find_package(LuaJit REQUIRED)

include_directories(${LUAJIT_INCLUDE_DIRS})

set(auv_server_src main.cpp application.cpp lua_config.cpp lua.cpp)

add_executable(auv_server ${auv_server_src})
target_link_libraries(auv_server auv_algorithm luajit-5.1)
