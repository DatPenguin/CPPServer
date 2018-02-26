cmake_minimum_required(VERSION 3.7.2)
project(CPPServer)

set(CMAKE_CXX_STANDARD 14)

set(HEADER_FILES includes/server.h
        includes/brickest.h)

set(SOURCE_FILES src/main.cpp
        src/clientio.cpp
        src/Spell.cpp
        includes/Spell.h
        includes/db.h
        src/util.cpp
        includes/util.h
        includes/clientio.h
        src/Fighter.cpp
        includes/Fighter.h
        src/Client.cpp
        includes/Client.h)

find_package(Threads)

add_executable(CPPServer ${SOURCE_FILES} ${HEADER_FILES})

target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -std=c++14)

target_link_libraries(CPPServer pqxx ${CMAKE_THREAD_LIBS_INIT})