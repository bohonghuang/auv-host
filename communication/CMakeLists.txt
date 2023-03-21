cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/vendor)
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/include)

set(auv_communication_src
        rov_control_block.cpp
        rov_controller.cpp lua_interop.cpp)

add_library(auv-communication STATIC ${auv_communication_src})
target_include_directories(auv-communication PUBLIC .)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
