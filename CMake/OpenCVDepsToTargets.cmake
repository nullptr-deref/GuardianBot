# Function searches for Debug and Release binaries of OpenCV dependencies
# Returns OPENCV_DEPS
function(OpenCVDepsToTargets PATH_TO_LIBS)
    message(STATUS "Searching for OpenCV dependencies under [${PATH_TO_LIBS}] ...")

    set(DEPENDENCIES "ade;IlmImf;ippicvmt;ippiw;ittnotify;libjpeg-turbo;libopenjp2;libpng;libprotobuf;libtiff;libwebp;quirc;zlib")

    file(COPY ${PATH_TO_LIBS}/../bin/opencv_videoio_ffmpeg450_64.dll DESTINATION ${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE})

    message(STATUS "=======================================================")
    foreach(LIBRARY_NAME IN LISTS DEPENDENCIES)
        find_library(DEBUG_LIB NAMES ${LIBRARY_NAME}d PATHS ${PATH_TO_LIBS} NO_DEFAULT_PATH)
        find_library(RELEASE_LIB NAMES ${LIBRARY_NAME} PATHS ${PATH_TO_LIBS} NO_DEFAULT_PATH)

        if(NOT ("${DEBUG_LIB}" STREQUAL "DEBUG_LIB-NOTFOUND") OR NOT ("${RELEASE_LIB}" STREQUAL "RELEASE_LIB-NOTFOUND"))
            message(STATUS "Found [${LIBRARY_NAME}]")
            message(STATUS "Debug: ${DEBUG_LIB}")
            message(STATUS "Release: ${RELEASE_LIB}")

            if("${DEBUG_LIB}" STREQUAL "DEBUG_LIB-NOTFOUND")
                set(DEBUG_LIB ${RELEASE_LIB} CACHE FILEPATH "Path to ${LIBRARY_NAME}" FORCE)
                message(STATUS "Debug binaries for [${LIBRARY_NAME}] were not found => replacing it with Release binaries...")
            endif()

            message(STATUS "Debug: ${DEBUG_LIB}")
            message(STATUS "Release: ${RELEASE_LIB}")

            add_library(${LIBRARY_NAME} STATIC IMPORTED GLOBAL)
            set_target_properties(${LIBRARY_NAME} PROPERTIES
                IMPORTED_LOCATION ${RELEASE_LIB}
                IMPORTED_LOCATION_DEBUG ${DEBUG_LIB}
                IMPORTED_CONFIGURATIONS "Release;Debug"
            )
            message(STATUS "Added target [${LIBRARY_NAME}] to global scope")

            list(APPEND TARGETS_LIST ${LIBRARY_NAME})

            unset(DEBUG_LIB CACHE)
            unset(RELEASE_LIB CACHE)
            message(STATUS "Unloaded library paths from cache for [${LIBRARY_NAME}]")
            message(STATUS "=======================================================")
        endif()
    endforeach()
    set(OPENCV_DEPS ${TARGETS_LIST} PARENT_SCOPE)
endfunction()