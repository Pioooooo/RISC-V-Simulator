cmake_minimum_required(VERSION 3.10)
project(RISC_V)

set(CMAKE_CXX_STANDARD 17)

add_executable(RISC_V main.cpp Simulator.cpp Simulator.h)