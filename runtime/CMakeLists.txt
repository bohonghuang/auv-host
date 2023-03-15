cmake_minimum_required(VERSION 3.10)

set(CMAKE_MODULE_PATH "../cmake/")

find_package(LuaJit REQUIRED)
include_directories(${LUAJIT_INCLUDE_DIRS})
include_directories(../vendor/sol2/include)
include_directories(../vendor/json-rpc-cxx/include)
include_directories(../vendor/json-rpc-cxx/vendor)

set(auv_connect_rov_src connect_rov.cpp)
set(auv_runtime_src main.cpp application.cpp block.cpp block.h)

add_library(auv_connect_rov STATIC ${auv_connect_rov_src})

add_executable(auv_runtime ${auv_runtime_src})
target_link_libraries(auv_runtime ${LUAJIT_LIBRARY} auv_vision auv_connect_rov)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
