# Copyright mzoesch. All rights reserved.

# Update to 4.0 when CLion supports it.
cmake_minimum_required(VERSION      3.31)

if(NOT DEFINED CMAKE_C_COMPILER)
    set(CMAKE_C_COMPILER "clang")
endif()
if(NOT DEFINED CMAKE_CXX_COMPILER)
    set(CMAKE_CXX_COMPILER "clang++")
endif()

string(REGEX MATCH ".*clang.*" REGREX_MATCHED ${CMAKE_C_COMPILER})
if(NOT REGREX_MATCHED)
    message(FATAL_ERROR "C compiler [Clang] is required, found: [${CMAKE_C_COMPILER}].")
endif()
string(REGEX MATCH ".*clang\\+\\+.*" REGREX_MATCHED ${CMAKE_CXX_COMPILER})
if(NOT REGREX_MATCHED)
    message(FATAL_ERROR "CXX compiler [Clang++] is required, found: [${CMAKE_CXX_COMPILER}].")
endif()

project(
    Reflex
    VERSION         0.0.1
    LANGUAGES       C CXX
    DESCRIPTION     ""
    HOMEPAGE_URL    "https://github.com/mzoesch/S-Jafg"
    )

find_package(Clang REQUIRED CONFIG)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_library(ReflexHook MODULE
    Launch.cpp
    )

set_target_properties(ReflexHook PROPERTIES
    COMPILE_FLAGS "-fno-rtti"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Binaries/Reflex"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Binaries/Reflex"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/../Binaries/Reflex"
    )
