cmake_minimum_required(VERSION 3.10)

project(4each C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -O2)

add_executable(4each foreach.c)