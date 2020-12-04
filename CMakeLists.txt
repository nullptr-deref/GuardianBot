cmake_minimum_required(VERSION 3.1)

if("${CMAKE_BUILD_TYPE}" STREQUAL "")
    set(CMAKE_BUILD_TYPE "Debug")
endif()

include(${CMAKE_SOURCE_DIR}/CMake/AddLibRealSense.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/AddOpenCV2.cmake)
include(${CMAKE_SOURCE_DIR}/CMake/OpenCVDepsToTargets.cmake)

project(GuardianBot LANGUAGES CXX)

add_executable(GuardianBotApp
    main.cpp
    ArgumentParser.cpp
)

OpenCVDepsToTargets(${OPENCV_LIBS_PATH})

FindOpenCVModule(dnn)
FindOpenCVModule(core)
FindOpenCVModule(imgproc)
FindOpenCVModule(highgui)
FindOpenCVModule(imgcodecs)

target_include_directories(GuardianBotApp PRIVATE ${librealsense2_INCLUDE_DIRS} ${opencv2_INCLUDE_DIRS})

target_link_libraries(GuardianBotApp
    PRIVATE
    librealsense2
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_dnn
    opencv_imgcodecs

    ${OPENCV_DEPS}
)

if(WIN32)
    target_compile_options(GuardianBotApp
        PRIVATE "/MTd"
    )
endif()