cmake_minimum_required(VERSION 3.10)

project(LINKED_LIST)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

include_directories(
    linked_list_backend/include
)

add_executable(linked_list_op_tests tests/src/linked_list_op_tests.cpp)

