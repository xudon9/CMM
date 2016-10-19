cmake_minimum_required(VERSION 3.0)

project(CMM)
set(SRC_LIST main.cpp CMMLexer.cpp)
add_executable(cmm ${SRC_LIST})

set_property(TARGET cmm PROPERTY CXX_STANDARD 11)
set(CXX_STANDARD_REQUIRED on)
