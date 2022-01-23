cmake_minimum_required(VERSION 3.11)

project("cli")

add_library("cli" STATIC
    "ArgumentParser.cpp"
)
set_target_properties("cli"
    PROPERTIES
    CXX_STANDARD_REQUIRED ON
    CXX_STANDARD 20
)

if(WIN32)
target_compile_options("cli"
    PRIVATE "/MTd"
)
endif(WIN32)
