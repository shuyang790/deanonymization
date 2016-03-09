cmake_minimum_required(VERSION 3.3)
project(deanonymization)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O3 -pthread")

set(SOURCE_FILES
    data_analysis.cpp
    graph.cpp
    main.cpp
    match.cpp)

include_directories(.)

add_executable(main ${SOURCE_FILES})