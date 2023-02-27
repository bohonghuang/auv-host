cmake_minimum_required(VERSION 3.10)

set(CMAKE_PREFIX_PATH "C:/msys64/usr/local/lib/cmake")

find_package(OpenCV REQUIRED)
find_package(sockpp REQUIRED)


include_directories(../vendor/lua)
include_directories(../vendor/json-rpc-cxx/include)
include_directories(../vendor/json-rpc-cxx/vendor)
include_directories(../vendor/sockpp/include)
include_directories(${OpenCV_INCLUDE_DIRS})

add_subdirectory(algorithm)

set(auv_server_src
        main.cpp
        connect_rov.cpp
        connect_client.cpp)

add_executable(auv_server ${auv_server_src})
target_link_libraries(auv_server ${OpenCV_LIBS} Sockpp::sockpp-static Ws2_32)
