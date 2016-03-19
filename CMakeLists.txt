cmake_minimum_required(VERSION 2.8)
project(deanonymization)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -pthread")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCE_FILES
    src/data_analysis.cpp
    src/graph.cpp
    src/main.cpp
    src/match.cpp)

include_directories(.)

# BASELINE switch
option(BASELINE "Run Baseline Algorithm" OFF)
if(BASELINE)
    add_definitions(-DBASELINE)
endif(BASELINE)

# TOPK switch
option(TOPK "Maintain only top-k nodes for each node in a graph" ON)
if(TOPK)
    add_definitions(-DTOPK)
endif(TOPK)

add_executable(main ${SOURCE_FILES})
