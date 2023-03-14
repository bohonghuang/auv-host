cmake_minimum_required(VERSION 3.10)

set(algorithm_srcs block.cpp camera.cpp red_bar.cpp utils.cpp red_door.cpp test/test.cpp)

add_library(auv_vision STATIC ${algorithm_srcs})
target_link_libraries(auv_vision ${OpenCV_LIBS})

add_executable(test test/test.cpp)
target_link_libraries(test auv_vision)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/vision/test)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/libs)
