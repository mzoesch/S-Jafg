# Copyright mzoesch. All rights reserved.

message(STATUS "Checking for in-source build ...")

# Throw if an in-source build is detected.
function(ThrowOnInSourceBuild)
    get_filename_component(SrcDir "${CMAKE_SOURCE_DIR}" REALPATH)
    get_filename_component(BinDir "${CMAKE_BINARY_DIR}" REALPATH)

    if("${SrcDir}" STREQUAL "${BinDir}")
        message(FATAL_ERROR "In-source build detected. Aborting CMake configuration.")
    endif()
endfunction()

ThrowOnInSourceBuild()
