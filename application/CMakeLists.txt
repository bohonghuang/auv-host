cmake_minimum_required(VERSION 3.10)

include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/vendor)
include_directories(${PROJECT_SOURCE_DIR}/vendor/json-rpc-cxx/include)
include_directories(${PROJECT_SOURCE_DIR}/vision)
include_directories(${LUAJIT_INCLUDE_DIRS})
include_directories(.)

set(auv_application_src
        main.cpp lua_interop.cpp)

add_executable(auv-application ${auv_application_src})
target_link_libraries(auv-application auv-runtime auv-communication auv-vision ${LUAJIT_LIBRARY})

set(auv_application_test_src
        ${auv_application_src}
        test/test.cpp)

list(REMOVE_ITEM auv_application_test_src main.cpp)

add_executable(auv-application-test ${auv_application_test_src})
target_link_libraries(auv-application-test PUBLIC auv-runtime auv-communication auv-vision PRIVATE Catch2::Catch2WithMain)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
