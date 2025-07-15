# Copyright mzoesch. All rights reserved.

if(CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "Multi-configuration generators are not allowed. Please use a single-configuration generator with -G <generator>.")
endif()
