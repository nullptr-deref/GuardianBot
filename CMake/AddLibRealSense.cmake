include(ExternalProject)

project(librealsense2 LANGUAGES C CXX)

ExternalProject_Add(
    librealsense2-external
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rd-party/librealsense2
    BINARY_DIR ${CMAKE_BINARY_DIR}/3rd-party/librealsense2/bin
    TMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/librealsense2/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/librealsense2/stamp
    LOG_DIR ${CMAKE_BINARY_DIR}/3rd-party/librealsense2/log
    INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd-party/librealsense2/install

    GIT_REPOSITORY https://github.com/IntelRealSense/librealsense
    GIT_TAG v2.39.0
    GIT_SHALLOW ON
    GIT_PROGRESS OFF
    BUILD_ALWAYS OFF

    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DBUILD_EXAMPLES=OFF
        -DBUILD_UNIT_TESTS=OFF
        -DBUILD_WITH_STATIC_CRT=ON
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
)

ExternalProject_Get_Property(librealsense2-external INSTALL_DIR)
set(LIBREALSENSE2_INSTALL_DIR ${INSTALL_DIR})

find_library(realsense_Debug NAMES realsense2d PATHS "${LIBREALSENSE2_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
find_library(realsense_Release NAMES realsense2 PATHS "${LIBREALSENSE2_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
find_library(realsense-file_Debug NAMES realsense-filed PATHS "${LIBREALSENSE2_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
find_library(realsense-file_Release NAMES realsense-file PATHS "${LIBREALSENSE2_INSTALL_DIR}/lib" NO_DEFAULT_PATH)

add_library(realsense-file STATIC IMPORTED)
set_target_properties(realsense-file PROPERTIES
    IMPORTED_LOCATION_DEBUG ${realsense-file_Debug}
    IMPORTED_LOCATION ${realsense-file_Release}
    IMPORTED_CONFIGURATIONS "Debug;Release"
)

add_library(librealsense2 STATIC IMPORTED)
set_target_properties(librealsense2 PROPERTIES
    IMPORTED_LOCATION_DEBUG ${realsense_Debug}
    IMPORTED_LOCATION ${realsense_Release}
    IMPORTED_CONFIGURATIONS "Debug;Release"
)
target_link_libraries(librealsense2 INTERFACE realsense-file)
set(librealsense2_INCLUDE_DIRS ${LIBREALSENSE2_INSTALL_DIR}/include)