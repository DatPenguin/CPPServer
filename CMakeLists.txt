cmake_minimum_required(VERSION 3.6.2)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        client.h
        server.h clientio.cpp dbconnection.cpp)
add_executable(CPPServer ${SOURCE_FILES})

target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -Wno-missing-field-initializers)