cmake_minimum_required(VERSION 3.8)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp
        client.h
        server.h)
add_executable(CPPServer ${SOURCE_FILES})

target_compile_options(CPPServer PRIVATE -Wall -Wextra)