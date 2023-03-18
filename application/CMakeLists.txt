cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/vendor)
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/include)
include_directories(${PROJECT_SOURCE_DIR}/vision)

include_directories(${LUAJIT_INCLUDE_DIRS})

set(auv_application_src
        main.cpp lua_interop.cpp)

add_executable(auv-application ${auv_application_src})
target_link_libraries(auv-application auv-runtime auv-communication auv-vision ${LUAJIT_LIBRARY})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
