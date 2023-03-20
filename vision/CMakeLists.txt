cmake_minimum_required(VERSION 3.10)

include_directories(.)
include_directories(${CMAKE_SOURCE_DIR})

set(auv_vision_srcs
        block/camera.cpp
        utils.cpp
        block/color.cpp
        block/find_bar.cpp
        block/detect.cpp
        block/camera_calibr.cpp
        block/imshow.cpp
        nn/model_lib.cpp
        nn/yolo_fast_v2.cpp
        lua_interop.cpp
        camera_mgr.cpp)

add_library(auv-vision STATIC ${auv_vision_srcs})
target_link_libraries(auv-vision ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

add_executable(auv-vision-test test/test.cpp)
target_link_libraries(auv-vision-test PUBLIC auv-vision PRIVATE Catch2::Catch2WithMain)
