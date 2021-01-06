cmake_minimum_required(VERSION 3.17)
project(templates_1)

set(CMAKE_CXX_STANDARD 17)

include_directories(.)

add_executable(templates_1
        main.cpp
        Parser.h
        ParserExceptions.cpp
        ParserExceptions.h
        printTuple.h)
