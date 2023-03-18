cmake_minimum_required(VERSION 3.10)

set(auv_runtime_src
        application.cpp)

add_library(auv_runtime STATIC ${auv_runtime_src})
target_link_libraries(auv_runtime ${LUAJIT_LIBRARY})

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
