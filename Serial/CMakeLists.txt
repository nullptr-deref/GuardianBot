cmake_minimum_required(VERSION 3.15)

project(Serial LANGUAGES CXX)

add_library(Serial STATIC
    SerialPort.cpp
)
set_target_properties(Serial PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)