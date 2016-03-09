cmake_minimum_required(VERSION 2.8)
project(deanonymization)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES
    data_analysis.cpp
    graph.cpp
    main.cpp
    match.cpp)

include_directories(.)

add_executable(main ${SOURCE_FILES})