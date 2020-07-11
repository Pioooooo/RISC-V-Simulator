cmake_minimum_required(VERSION 3.10)
project(RISC_V)
project(code)

set(CMAKE_CXX_STANDARD 17)
file(GLOB src ${PROJECT_SOURCE_DIR}/src/*.cpp)

add_executable(RISC_V ${src})
add_executable(code ${src})