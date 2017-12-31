cmake_minimum_required(VERSION 3.7.2)
project(CPPServer)

set(CMAKE_CXX_STANDARD 14)

set(HEADER_FILES includes/server.h
        includes/brickest.h)

set(SOURCE_FILES main.cpp
        clientio.cpp
        Spell.cpp
        includes/Spell.h
        includes/db.h
        util.cpp
        includes/util.h
        includes/clientio.h
        Fighter.cpp
        includes/Fighter.h
        Client.cpp
        includes/Client.h)

add_executable(CPPServer ${SOURCE_FILES} ${HEADER_FILES})

target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -std=c++14)

target_link_libraries(CPPServer pqxx)