cmake_minimum_required(VERSION 3.1)

project(gl LANGUAGES CXX)

add_library(gl STATIC
    glstuff.cpp
    VertexArray.cpp
    VertexArrayLayout.cpp
    Program.cpp
    Shader.cpp
)

find_package(OpenGL REQUIRED)

target_include_directories(gl PRIVATE
    ${glew_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${opencv2_INCLUDE_DIRS}
)

target_link_libraries(gl glfw glew OpenGL::GL)
set_target_properties(gl PROPERTIES
    CXX_STANDARD_REQUIRED TRUE
    CXX_STANDARD 17
)

if(WIN32)
    target_compile_options(gl
        PRIVATE "/MTd"
    )
endif()