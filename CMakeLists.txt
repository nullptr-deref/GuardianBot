cmake_minimum_required(VERSION 3.1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(${CMAKE_SOURCE_DIR}/CMake/AddLibRealSense.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/AddOpenCV2.cmake)

project(Test LANGUAGES CXX)

add_executable(Test main.cpp)

FindOpenCVModule(dnn)
FindOpenCVModule(core)
FindOpenCVModule(imgproc)
FindOpenCVModule(highgui)

target_include_directories(Test PRIVATE ${librealsense2_INCLUDE_DIRS} ${opencv2_INCLUDE_DIRS})

target_link_libraries(Test
    librealsense2
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_dnn
)

if(WIN32)
    target_compile_options(Test
        PRIVATE "/MTd"
    )
endif()