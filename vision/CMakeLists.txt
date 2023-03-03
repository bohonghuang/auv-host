cmake_minimum_required(VERSION 3.10)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})

set(algorithm_srcs block.cpp camera.cpp)

add_library(auv_algorithm STATIC ${algorithm_srcs})
target_link_libraries(auv_algorithm ${OpenCV_LIBS})
