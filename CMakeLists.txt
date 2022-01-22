cmake_minimum_required(VERSION 3.11)

if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
    set(CMAKE_BUILD_TYPE "Debug")
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )

project("GuardianBot")

include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
find_package(OpenGL REQUIRED)
find_package(realsense2 2.49.0 EXACT REQUIRED)
find_package(opencv 4.5.0 EXACT REQUIRED)
find_package(glfw3 3.3.2 EXACT REQUIRED)
find_package(GLEW 2.2.0 EXACT REQUIRED)
find_package(imgui 1.80 REQUIRED)

add_subdirectory("vidIO")
add_subdirectory("Serial")
add_subdirectory("gl")

add_executable(GuardianBotApp
    "main.cpp"
    "ArgumentParser.cpp"
    "Camera.cpp"
    "backends/imgui_impl_glfw.cpp"
    "backends/imgui_impl_opengl3.cpp"
    "backends/imgui_impl_glfw.h"
    "backends/imgui_impl_opengl3.h"
)

target_compile_definitions(GuardianBotApp PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
target_include_directories(GuardianBotApp
    PRIVATE
    ${imgui_INCLUDE_DIRS}
    ${realsense2_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OpenGL_INCLUDE_DIRS}

    ${CMAKE_SOURCE_DIR}/Serial
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(GuardianBotApp
    PRIVATE
    realsense2::realsense2
    opencv::opencv
    glfw::glfw
    GLEW::GLEW
    vidIO
    Serial
    imgui::imgui

    gl
    OpenGL::GL
)

set_target_properties(GuardianBotApp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

if( WIN32 )
    target_compile_options(GuardianBotApp
        PRIVATE "/MTd"
    )
endif( WIN32 )

message(STATUS "Copying default shaders source code into working directory...")
file(COPY
    ${CMAKE_SOURCE_DIR}/resources/VertexDefault.shader;
    ${CMAKE_SOURCE_DIR}/resources/FragmentDefault.shader
    DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resources
)
file(INSTALL
    "${CMAKE_BINARY_DIR}/bin/"
    DESTINATION ${CMAKE_BUILD_TYPE}
    FILES_MATCHING PATTERN "*.dll"
)