cmake_minimum_required(VERSION 3.10)

project(auv-host)

add_subdirectory(runtime)
add_subdirectory(vision)
add_subdirectory(application)

set(EXECUTABLE_OUTPUT_PATH .)
set(LIBRARY_OUTPUT_PATH  libs)
