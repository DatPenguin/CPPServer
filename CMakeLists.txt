cmake_minimum_required(VERSION 3.7.2)
project(CPPServer)

set(CMAKE_CXX_STANDARD 14)

set(HEADER_FILES includes/engine.h
        includes/brickest.h src/QClient.cpp includes/QClient.h src/Stats.cpp includes/Stats.h src/engine.cpp src/query.cpp includes/query.h)

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
        includes/Client.h src/QClient.cpp includes/QClient.h src/Stats.cpp includes/Stats.h src/engine.cpp src/query.cpp includes/query.h)

find_package(Threads)

add_executable(CPPServer ${SOURCE_FILES} ${HEADER_FILES})

target_compile_options(CPPServer PRIVATE -Wall -W -Wextra -ansi -pedantic -std=c++14)

target_link_libraries(CPPServer pqxx ${CMAKE_THREAD_LIBS_INIT})