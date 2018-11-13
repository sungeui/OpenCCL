cmake_minimum_required(VERSION 3.0)
project(libsrc)

set(CMAKE_BUILD_TOOL Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(../include ./ ../)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2")

link_directories(./ ../)


add_library(OpenCCL LayoutGraph.cpp LayoutMetrics.cpp)

install(TARGETS OpenCCL DESTINATION ../Lib)