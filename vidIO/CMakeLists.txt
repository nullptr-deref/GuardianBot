cmake_minimum_required(VERSION 3.1)

project(vidIO LANGUAGES CXX)

add_library(vidIO STATIC
    Camera.cpp
    RSCameraAdapter.cpp
    CVCameraAdapter.cpp
)

target_include_directories(vidIO PRIVATE ${opencv_INCLUDE_DIRS} ${realsense2_INCLUDE_DIRS})

target_link_libraries(vidIO
    opencv::opencv
    realsense2::realsense2
)

if(WIN32)
target_compile_options(vidIO
    PRIVATE "/MTd"
)
endif()