cmake_minimum_required(VERSION 3.10)

project(ImageCompare LANGUAGES CXX)

add_library(imgcmp imgcmp.cpp)

target_include_directories(imgcmp PRIVATE ${opencv2_INCLUDE_DIRS})

if(WIN32)
    target_compile_options(imgcmp
        PRIVATE "/MTd"
    )
endif()

target_link_libraries(imgcmp
    PRIVATE
    opencv_core
    opencv_imgproc
    opencv_imgcodecs

    ${OPENCV_DEPS}
)