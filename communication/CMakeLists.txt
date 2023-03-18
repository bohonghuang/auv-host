cmake_minimum_required(VERSION 3.10)

include_directories(../vendor/json-rpc-cxx/vendor)
include_directories(../vendor/json-rpc-cxx/include)

set(auv_communication_src
        rov_control_block.cpp
        rov_controller.cpp)

add_library(auv_communication STATIC ${auv_communication_src})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

