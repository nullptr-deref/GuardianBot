cmake_minimum_required(VERSION 3.1)

project(vidIO LANGUAGES CXX)

add_library(vidIO STATIC
    Camera.cpp
    RSCameraAdapter.cpp
    CVCameraAdapter.cpp
)

target_include_directories(vidIO PRIVATE ${opencv2_INCLUDE_DIRS} ${librealsense2_INCLUDE_DIRS})

target_link_libraries(vidIO
    opencv_videoio
    opencv_imgproc
    librealsense2
)

if(WIN32)
target_compile_options(vidIO
    PRIVATE "/MTd"
)
endif()