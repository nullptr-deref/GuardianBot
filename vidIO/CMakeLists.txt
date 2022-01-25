cmake_minimum_required(VERSION 3.15)

project(vidIO LANGUAGES CXX)

add_library(vidIO STATIC
    Camera.cpp
    CVCameraAdapter.cpp
)

target_include_directories(vidIO PRIVATE ${opencv_INCLUDE_DIRS})

target_link_libraries(vidIO
    opencv::opencv
)