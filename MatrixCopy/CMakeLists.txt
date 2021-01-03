cmake_minimum_required(VERSION 3.10)

project(MatrixCopy LANGUAGES CXX)

add_library(matrixcpy STATIC MatrixCopy.cpp)

target_include_directories(matrixcpy
    PRIVATE
    ${opencv2_INCLUDE_DIRS}
)

target_link_libraries(matrixcpy
    PRIVATE
    opencv_core
    opencv_imgproc
    opencv_highgui
    opencv_imgcodecs

    ${OPENCV_DEPS}
)

if(WIN32)
    target_compile_options(matrixcpy
        PRIVATE "/MTd"
    )
endif()