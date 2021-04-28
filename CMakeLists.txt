cmake_minimum_required(VERSION 3.19)
project(ICP)

set(CMAKE_CXX_STANDARD 17)

add_executable(ICP main.cpp MQTTheader.h MQTTheader.cpp)