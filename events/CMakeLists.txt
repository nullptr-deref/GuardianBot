cmake_minimum_required(VERSION 3.10)

project(events LANGUAGES CXX)

add_library(events STATIC
    Listener.cpp
)

set_target_properties(events PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

if(WIN32)
    target_compile_options(events
        PRIVATE "/MTd"
    )
endif()