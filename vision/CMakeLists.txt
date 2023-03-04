cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(algorithm_srcs block.cpp camera.cpp red_bar.cpp utils.cpp)

add_library(auv_vision STATIC ${algorithm_srcs})
target_link_libraries(auv_vision ${OpenCV_LIBS})

add_executable(test_red_bar test/red_bar.cpp)
target_link_libraries(test_red_bar auv_vision)
