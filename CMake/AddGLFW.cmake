include(ExternalProject)

project(glfw LANGUAGES C CXX)

ExternalProject_Add(
    glfw-external
    SOURCE_DIR ${CMAKE_SOURCE_DIR}/3rd-party/glfw
    BINARY_DIR ${CMAKE_BINARY_DIR}/3rd-party/glfw/bin
    TMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/glfw/tmp
    STAMP_DIR ${CMAKE_BINARY_DIR}/3rd-party/glfw/stamp
    LOG_DIR ${CMAKE_BINARY_DIR}/3rd-party/glfw/log
    INSTALL_DIR ${CMAKE_BINARY_DIR}/3rd-party/glfw/install

    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.3.2
    GIT_SHALLOW ON
    GIT_PROGRESS OFF
    BUILD_ALWAYS OFF

    CMAKE_ARGS
        -DBUILD_SHARED_LIBS=OFF
        -DGLFW_BUILD_EXAMPLES=OFF
        -DGLFW_BUILD_TESTS=OFF
        -DGLFW_BUILD_DOCS=OFF
        -DGLFW_INSTALL=ON
        -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
        -DUSE_MSVC_RUNTIME_LIBRARY_DLL=OFF
)

ExternalProject_Get_Property(glfw-external INSTALL_DIR)
set(GLFW_INSTALL_DIR ${INSTALL_DIR})

find_library(glfw_lib NAMES glfw3 PATHS "${GLFW_INSTALL_DIR}/lib")

add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
    IMPORTED_LOCATION_DEBUG ${glfw_lib}
    IMPORTED_LOCATION ${glfw_lib}
    IMPORTED_CONFIGURATIONS "Debug;Release"
)

set(glfw_INCLUDE_DIRS ${GLFW_INSTALL_DIR}/include)