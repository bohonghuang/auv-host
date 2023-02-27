cmake_minimum_required(VERSION 3.10)
project(auv-host-v2)

add_subdirectory(vendor/sockpp)
add_subdirectory(server)
add_subdirectory(client)

set(EXECUTABLE_OUTPUT_PATH .)
set(LIBRARY_OUTPUT_PATH  ./libs)
