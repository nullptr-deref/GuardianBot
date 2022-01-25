cmake_minimum_required(VERSION 3.15)

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
    ${GLEW_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${CMAKE_SOURCE_DIR}
    ${opencv_INCLUDE_DIRS}
)

target_link_libraries(gl glfw::glfw GLEW::GLEW OpenGL::GL)
set_target_properties(gl PROPERTIES
    CXX_STANDARD_REQUIRED TRUE
    CXX_STANDARD 17
)