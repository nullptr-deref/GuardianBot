cmake_minimum_required(VERSION 3.10)

project(Serial LANGUAGES CXX)

add_library(Serial STATIC
    SerialPort.cpp
    SerialPortImpl.cpp
)
set_target_properties(Serial PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON)

if(WIN32)
        target_compile_options(Serial
            PRIVATE "/MTd"
        )
    endif()