cmake_minimum_required(VERSION 3.10)

include_directories(../)
include_directories(../vendor/json-rpc-cxx/vendor)
include_directories(../vendor/json-rpc-cxx/include)

include_directories(${LUAJIT_INCLUDE_DIRS})

set(auv_application_src
        main.cpp lua_interop.cpp)

add_executable(auv_application ${auv_application_src})
target_link_libraries(auv_application auv_runtime auv_communication auv_vision ${LUAJIT_LIBRARY})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
