cmake_minimum_required(VERSION 3.10)

find_package(sockpp REQUIRED)

set(auv_client_src
        main.cpp
        connect_server.cpp)

add_executable(auv_client ${auv_client_src})
target_link_libraries(auv_client Sockpp::sockpp-static Ws2_32)