cmake_minimum_required(VERSION 3.15)
project(lab5_part3 C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_FLAGS "-pthread")

add_executable(8_client 8_client.c)
add_executable(8_server 8_server.c)