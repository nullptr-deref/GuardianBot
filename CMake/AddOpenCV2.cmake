include(ExternalProject)

project(opencv2 LANGUAGES C CXX)

ExternalProject_Add(
    opencv-external
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rd-party/opencv
    BINARY_DIR ${CMAKE_BINARY_DIR}/3rd-party/opencv/bin
    TMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/opencv/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/opencv/stamp
    LOG_DIR ${CMAKE_BINARY_DIR}/3rd-party/opencv/log
    INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd-party/opencv/install

    GIT_REPOSITORY https://github.com/opencv/opencv
    GIT_TAG 4.5.0
    GIT_SHALLOW ON
    GIT_PROGRESS OFF
    BUILD_ALWAYS OFF

    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_opencv_apps=OFF
        -DBUILD_opencv_python2=OFF
        -DBUILD_opencv_python3=OFF
        -DBUILD_JAVA=OFF
        -DINSTALL_C_EXAMPLES=OFF
        -DINSTALL_TESTS=OFF
        -DBUILD_PERF_TESTS=OFF
        -DBUILD_TESTS=OFF
        -DBUILD_DOCS=OFF
        -DBUILD_EXAMPLES=OFF
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DOpenCV_DIR=<INSTALL_DIR>
)

ExternalProject_Get_Property(opencv-external INSTALL_DIR)
set(OPENCV_INSTALL_DIR ${INSTALL_DIR})
set(OPENCV_LIBS_PATH ${INSTALL_DIR}/x64/vc16/staticlib)

function(FindOpenCVModule MODULE_NAME)
    message(STATUS "===================== OpenCVModuleSearcher =====================")
    message(STATUS "Searching for OpenCV ${MODULE_NAME} module...")
    set(TARGET_NAME "opencv_${MODULE_NAME}")
    set(LIBRARY_NAME "opencv_${MODULE_NAME}450")

    find_library(DEBUG_LIB NAMES ${LIBRARY_NAME}d PATHS ${OPENCV_LIBS_PATH} NO_DEFAULT_PATH)
    find_library(RELEASE_LIB NAMES ${LIBRARY_NAME} PATHS ${OPENCV_LIBS_PATH} NO_DEFAULT_PATH)

    message(STATUS "Found Debug OpenCV [${MODULE_NAME}]")
    message(STATUS "Debug: ${DEBUG_LIB}")
    message(STATUS "Release: ${RELEASE_LIB}")

    message(STATUS "Specified target [${TARGET_NAME}] for found OpenCV module")
    add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    set_target_properties(${TARGET_NAME} PROPERTIES
        IMPORTED_LOCATION_DEBUG ${DEBUG_LIB}
        IMPORTED_LOCATION ${RELEASE_LIB}
        IMPORTED_CONFIGURATIONS "Debug;Release"
    )

    unset(DEBUG_LIB CACHE)
    unset(RELEASE_LIB CACHE)
    message(STATUS "Unloaded paths to [${MODULE_NAME}] libraries")
    message(STATUS "===================== OpenCVModuleSearcher End =====================")
endfunction()

set(opencv2_INCLUDE_DIRS ${OPENCV_INSTALL_DIR}/include)