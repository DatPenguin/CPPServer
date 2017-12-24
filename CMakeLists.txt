cmake_minimum_required(VERSION 3.7.2)
project(CPPServer)

#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

set(HEADER_FILES includes/server.h
        includes/brickest.h)

set(SOURCE_FILES main.cpp
        clientio.cpp
        dbconnection.cpp
        spell.cpp includes/spell.h includes/db.h util.cpp includes/util.h includes/clientio.h)

add_executable(CPPServer ${SOURCE_FILES} ${HEADER_FILES})

#find_package(PostgreSQL REQUIRED)

#target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -Wno-missing-field-initializers -std=c++1y)
target_compile_options(CPPServer PRIVATE -Wall -Wextra -ansi -pedantic -Wno-missing-field-initializers -std=c++14)

target_link_libraries(CPPServer pqxx)