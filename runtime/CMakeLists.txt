cmake_minimum_required(VERSION 3.10)

include_directories(${Readline_INCLUDE_DIR})

set(auv_runtime_src
        application.cpp lua_interop.cpp lua_block.cpp lua_block.h)

add_library(auv-runtime STATIC ${auv_runtime_src})
target_link_libraries(auv-runtime ${LUAJIT_LIBRARY} ${Readline_LIBRARY})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
