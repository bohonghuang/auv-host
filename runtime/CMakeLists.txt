cmake_minimum_required(VERSION 3.10)

include_directories(.)
include_directories(${Readline_INCLUDE_DIR})

set(auv_runtime_src
        application.cpp lua_interop.cpp lua_block.cpp concurrent.cpp)

add_library(auv-runtime STATIC ${auv_runtime_src})
target_link_libraries(auv-runtime pthread ${LUAJIT_LIBRARY} ${Readline_LIBRARY})

add_executable(auv-runtime-test test/test.cpp)
target_link_libraries(auv-runtime-test PUBLIC auv-runtime PRIVATE Catch2::Catch2WithMain)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
