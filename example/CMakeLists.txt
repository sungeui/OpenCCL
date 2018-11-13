cmake_minimum_required(VERSION 3.0)
project(example)

set(CMAKE_BUILD_TOOL Release)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# What optimization level to use
set(OPTFLAGS "-O2")
# What options to be used by the compiler
set(COPTIONS "")
# What options to be used by the loader
set(LDOPTIONS "")
# What archiving to use
set(AR = "ar rv")

INCLUDE_DIRECTORIES(../include ./ ../)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${COPTIONS} ${OPTFLAGS}")

link_directories(../Lib ../libsrc)
link_libraries(OpenCCL metis)

add_executable(Demo Demo.cpp)