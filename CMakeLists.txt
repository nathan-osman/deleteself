cmake_minimum_required(VERSION 3.10)
project(deleteself)

find_program(NASM nasm)

if(NASM)
    message(STATUS "NASM compiler found: ${NASM}")
else()
    message(FATAL_ERROR "NASM compiler was not found")
endif()

add_subdirectory(src)
add_subdirectory(stub)
add_subdirectory(test)
