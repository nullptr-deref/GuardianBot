cmake_minimum_required(VERSION 3.1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(${CMAKE_SOURCE_DIR}/CMake/AddLibRealSense.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/AddOpenCV2.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/OpenCVDepsToTargets.cmake)

include(${CMAKE_SOURCE_DIR}/CMake/AddGLFW.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/AddGlew.cmake)

project(GuardianBot LANGUAGES C CXX)

set(LIBREALSENSE_BUILT FALSE CACHE BOOL "Represents whether librealsense external project is built or not")
set(OPENCV_BUILT FALSE CACHE BOOL "Represents whether opencv external project is built or not")

set(LIBREALSENSE_INCLUDE_ROOT "${CMAKE_BINARY_DIR}/3rd-party/librealsense2/install/include")
set(OPENCV_INCLUDE_ROOT "${CMAKE_BINARY_DIR}/3rd-party/opencv/install/include")

option(GB_SHARED_GL "Whether project should use static or shared OpenGL dependencies" TRUE)

if(EXISTS ${LIBREALSENSE_INCLUDE_ROOT})
    unset(LIBREALSENSE_BUILT CACHE)
    set(LIBREALSENSE_BUILT TRUE CACHE BOOL "Represents whether librealsense external project is built or not" FORCE)
endif()
if(EXISTS ${OPENCV_INCLUDE_ROOT})
    unset(OPENCV_BUILT CACHE)
    set(OPENCV_BUILT TRUE CACHE BOOL "Represents wether opencv external project is built or not" FORCE)
endif()

if(${LIBREALSENSE_BUILT} AND ${OPENCV_BUILT})
    add_subdirectory(Serial)
    add_subdirectory(gl)

    include(${CMAKE_SOURCE_DIR}/CMake/BuildDearImGui.cmake)
    
    add_executable(GuardianBotApp
        main.cpp
        ArgumentParser.cpp
        Camera.cpp
    )

    OpenCVDepsToTargets(${OPENCV_LIBS_PATH})

    FindOpenCVModule(dnn)
    FindOpenCVModule(core)
    FindOpenCVModule(imgproc)
    FindOpenCVModule(highgui)
    FindOpenCVModule(imgcodecs)

    find_package(OpenGL REQUIRED)

    target_include_directories(GuardianBotApp
        PRIVATE
        ${librealsense2_INCLUDE_DIRS}
        ${opencv2_INCLUDE_DIRS}
        ${glew_INCLUDE_DIRS}
        ${glfw_INCLUDE_DIRS}
        ${OpenGL_INCLUDE_DIRS}

        ${CMAKE_SOURCE_DIR}/ImageCompare
        ${CMAKE_SOURCE_DIR}/Serial
        ${CMAKE_SOURCE_DIR}/events
        ${CMAKE_SOURCE_DIR}

        ${DearImGui_INCLUDE_DIRS}
    )

    target_link_libraries(GuardianBotApp
        PRIVATE
        librealsense2
        opencv_core
        opencv_imgproc
        opencv_highgui
        opencv_dnn
        opencv_imgcodecs

        gl
        glew
        glfw
        OpenGL::GL

        dearimgui

        Serial

        ${OPENCV_DEPS}
    )

    if (NOT ${GB_SHARED_GL})
        target_compile_definitions(GuardianBotApp PRIVATE GLEW_STATIC)
    endif()

    set_target_properties(GuardianBotApp PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED ON
    )

    if(WIN32)
        target_compile_options(GuardianBotApp
            PRIVATE "/MTd"
        )
    endif()

    set(GLFW_DLL_NAME glfw3.dll)
    if(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
        set(GLEW_DLL_NAME glew32d.dll)
    else()
        set(GLEW_DLL_NAME glew32.dll)
    endif()
    message(STATUS "Copying default shaders source code into working directory...")
    file(COPY
        ${CMAKE_SOURCE_DIR}/resources/VertexDefault.shader;${CMAKE_SOURCE_DIR}/resources/FragmentDefault.shader
        DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}
    )
    file(COPY ${glew_DLL_DIR}/${GLEW_DLL_NAME};${glfw_DLL_DIR}/${GLFW_DLL_NAME} DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})
else()
    message(WARNING "Whether librealsense2 or opencv built copies were not found.")
    message(WARNING "Make sure you've built it using librealsense2-external and opencv-external targets of the project first.")
endif()