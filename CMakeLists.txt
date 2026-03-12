cmake_minimum_required(VERSION 3.16)
project(socketcan_toolkit C)
set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(can_send src/can_send.c)
add_executable(can_dump src/can_dump.c)