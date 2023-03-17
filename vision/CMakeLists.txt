cmake_minimum_required(VERSION 3.10)

include_directories(.)

set(algorithm_srcs
        block/camera.cpp
        utils.cpp
        block/color.cpp
        nn/yolo_fast_v2.cpp
        block/find_bar.cpp
        block/biology.cpp
        block/camera_calibr.cpp block/imshow.h block/imshow.cpp)

add_library(auv_vision STATIC ${algorithm_srcs})
target_link_libraries(auv_vision ${OpenCV_LIBS})

add_executable(test test/test.cpp)
target_link_libraries(test auv_vision)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
