include(ExternalProject)

project(glfw LANGUAGES C CXX)

ExternalProject_Add(
    glew-external
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rd-party/glew
    BINARY_DIR ${CMAKE_BINARY_DIR}/3rd-party/glew/bin
    TMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/glew/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/glew/stamp
    LOG_DIR ${CMAKE_BINARY_DIR}/3rd-party/glew/log
    INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd-party/glew/install

    URL https://github.com/nigels-com/glew/releases/download/glew-2.1.0/glew-2.1.0.zip

    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -D BUILD_UTILS=OFF
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
    
    SOURCE_SUBDIR "build/cmake"
)

ExternalProject_Get_Property(glew-external INSTALL_DIR)
set(GLEW_INSTALL_DIR ${INSTALL_DIR})

find_library(glew_Debug NAMES libglew32d PATHS "${GLEW_INSTALL_DIR}/lib" NO_DEFAULT_PATH)
find_library(glew_Release NAMES libglew32 PATHS "${GLEW_INSTALL_DIR}/lib" NO_DEFAULT_PATH)

add_library(glew STATIC IMPORTED)
set_target_properties(glew PROPERTIES
    IMPORTED_LOCATION_DEBUG ${glew_Debug}
    IMPORTED_LOCATION ${glew_Release}
    IMPORTED_CONFIGURATIONS "Debug;Release"
)

set(glew_INCLUDE_DIRS ${GLEW_INSTALL_DIR}/include)