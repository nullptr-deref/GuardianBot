cmake_minimum_required(VERSION 3.15)

if( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
    set(CMAKE_BUILD_TYPE "Debug")
endif( "${CMAKE_BUILD_TYPE}" STREQUAL "" )
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

project("GuardianBot")
option(GB_PROFILER_MODE "Enables application profiling" FALSE)

include("${CMAKE_BINARY_DIR}/conan_paths.cmake")
find_package(OpenGL REQUIRED)
find_package(opencv 4.5.0 EXACT REQUIRED)
find_package(glfw3 3.3.2 EXACT REQUIRED)
find_package(GLEW 2.2.0 EXACT REQUIRED)
find_package(imgui 1.80 REQUIRED)
find_package(easy_profiler 2.1.0 EXACT REQUIRED)
find_package(spdlog 1.9.2 EXACT REQUIRED)

add_subdirectory("vidIO")
add_subdirectory("Serial")
add_subdirectory("gl")
add_subdirectory("cli")

add_executable(GuardianBotApp
    "main.cpp"
    "backends/imgui_impl_glfw.cpp"
    "backends/imgui_impl_opengl3.cpp"
    "backends/imgui_impl_glfw.h"
    "backends/imgui_impl_opengl3.h"
)

target_compile_definitions(GuardianBotApp PUBLIC IMGUI_IMPL_OPENGL_LOADER_GLEW)
if (${GB_PROFILER_MODE})
    target_compile_definitions(GuardianBotApp PUBLIC PROFILING)
endif(${GB_PROFILER_MODE})
target_include_directories(GuardianBotApp
    PRIVATE
    ${opencv_INCLUDE_DIRS}
    ${imgui_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${GLEW_INCLUDE_DIRS}
    ${OpenGL_INCLUDE_DIRS}
    ${easy_profiler_INCLUDE_DIRS}
    ${spdlog_INCLUDE_DIRS}

    ${CMAKE_SOURCE_DIR}/Serial
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(GuardianBotApp
    PRIVATE
    opencv::opencv
    glfw::glfw
    GLEW::GLEW
    imgui::imgui
    easy_profiler
    spdlog::spdlog
    cli
    vidIO
    Serial

    gl
    OpenGL::GL
)

set_target_properties(GuardianBotApp PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
)

message(STATUS "Copying default shaders source code into working directory...")
file(INSTALL
    ${CMAKE_SOURCE_DIR}/resources/VertexDefault.shader;
    ${CMAKE_SOURCE_DIR}/resources/FragmentDefault.shader
    DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/resources
)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    file(INSTALL
        "${CMAKE_BINARY_DIR}/bin/"
        DESTINATION ${CMAKE_BUILD_TYPE}
        FILES_MATCHING PATTERN "*d.dll"
    )
else()
file(INSTALL
    "${CMAKE_BINARY_DIR}/bin/"
    DESTINATION ${CMAKE_BUILD_TYPE}
    FILES_MATCHING PATTERN "*.dll"
)
endif()
file(INSTALL
    "${CMAKE_BINARY_DIR}/bin/imgui.dll"
    "${CMAKE_BINARY_DIR}/bin/glfw3.dll"
    DESTINATION ${CMAKE_BUILD_TYPE}
)