cmake_minimum_required(VERSION 3.6.2)
project(CPPServer)

set(CMAKE_CXX_STANDARD 11)

set(HEADER_FILES includes/server.h
        includes/brickest.h
        includes/fighter.h)

set(SOURCE_FILES main.cpp
        clientio.cpp
        dbconnection.cpp
        spell.cpp includes/spell.h includes/db.h)

add_executable(CPPServer ${SOURCE_FILES} ${HEADER_FILES})

#find_package(PostgreSQL REQUIRED)

target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -Wno-missing-field-initializers -std=c++1y)

target_link_libraries(CPPServer pqxx)