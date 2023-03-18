cmake_minimum_required(VERSION 3.10)

include_directories(.)

set(auv_vision_srcs
        block/camera.cpp
        utils.cpp
        block/color.cpp
        block/find_bar.cpp
        block/biology.cpp
        block/camera_calibr.cpp
        block/imshow.cpp
        nn/model_lib.cpp
        nn/yolo_fast_v2.cpp)

add_library(auv_vision STATIC ${auv_vision_srcs})
target_link_libraries(auv_vision ${OpenCV_LIBS})

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
