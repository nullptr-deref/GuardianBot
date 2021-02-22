project(DearImGui LANGUAGES CXX)

set(DearImGui_INCLUDE_DIRS "${CMAKE_SOURCE_DIR}/imgui;${CMAKE_SOURCE_DIR}/imgui/backends")
# I'm building imgui for using it with glfw and OpenGL3 so I'm building suitable backends for it
set(DearImGui_BACKEND_SRCS "${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_glfw.cpp;${CMAKE_SOURCE_DIR}/imgui/backends/imgui_impl_opengl3.cpp")
file(GLOB DearImGui_SRCS "imgui/*.cpp")

add_library(dearimgui STATIC ${DearImGui_BACKEND_SRCS} ${DearImGui_SRCS})
target_include_directories(dearimgui PRIVATE
    ${DearImGui_INCLUDE_DIRS}
    ${glfw_INCLUDE_DIRS}
    ${glew_INCLUDE_DIRS}
)
target_compile_definitions(dearimgui PRIVATE IMGUI_IMPL_OPENGL_LOADER_GLEW)
if(NOT ${GB_SHARED_GL})
    target_compile_definitions(dearimgui PRIVATE GLEW_STATIC)
endif()