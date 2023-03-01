cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "../vendor/")

find_package(sockpp REQUIRED)
find_package(LuaJit REQUIRED)

include_directories(../vendor/json-rpc-cxx/include)
include_directories(../vendor/json-rpc-cxx/vendor)
include_directories(../vendor/sockpp/include)
include_directories(${LUAJIT_INCLUDE_DIRS})

add_subdirectory(algorithm)

set(auv_server_src
        main.cpp
        connect_rov.cpp
        connect_client.cpp application.cpp lua_config.cpp lua.cpp)

add_executable(auv_server ${auv_server_src})
target_link_libraries(auv_server auv_algorithm Sockpp::sockpp-static luajit-5.1 Ws2_32)
