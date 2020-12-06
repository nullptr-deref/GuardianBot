cmake_minimum_required(VERSION 3.10)

project(Serial LANGUAGES CXX)

add_library(Serial
    SerialPort.cpp
    SerialPortImpl.cpp
)