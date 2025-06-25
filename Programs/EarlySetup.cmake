# Copyright mzoesch. All rights reserved.

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "ENGINE_ROOT: ${JAFG_ENGINE_ROOT}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

#
# The motor of Jafg.
#
set(JAFG_MOTOR_TARGET_CONFIG "release" CACHE STRING "Target config for motor")
set_property(CACHE JAFG_MOTOR_TARGET_CONFIG PROPERTY STRINGS debug release)
set(JAFG_MOTOR_DIR "${JAFG_ENGINE_ROOT}/Motor")
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(JAFG_MOTOR_EXECUTABLE "${JAFG_MOTOR_DIR}/target/${JAFG_MOTOR_TARGET_CONFIG}/motor")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(JAFG_MOTOR_EXECUTABLE "${JAFG_MOTOR_DIR}/target/${JAFG_MOTOR_TARGET_CONFIG}/motor.exe")
endif()
if(NOT DEFINED JAFG_MOTOR_EXECUTABLE)
    message(FATAL_ERROR "JAFG_MOTOR_EXECUTABLE is not defined. Please define it with -DJAFG_MOTOR_EXECUTABLE=<path to motor executable>.")
endif()
message(STATUS "JAFG_MOTOR_EXECUTABLE: ${JAFG_MOTOR_EXECUTABLE}")
set(JAFG_MOTOR_TOML "${JAFG_MOTOR_DIR}/Cargo.toml")

include(Programs/ThrowOnInSourceBuild.cmake)
include(Programs/ThrowOnMultiConfiguration.cmake)

# CMake extensions.
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

if(NOT EXISTS "${JAFG_ENGINE_ROOT}/jafg.jafgworkspace")
    message(FATAL_ERROR "Jafg engine root directory was not found. Falsely evaluated to: [${JAFG_ENGINE_ROOT}].")
endif()

#
# The performance cost is neglectable for the insane amount of benefits it brings.
# So just enable globally (eww global settings; god redeem us for our global preferences).
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Utils
include(Programs/Json.cmake)

if(NOT DEFINED JAFG_MOTOR_EXECUTABLE)
    message(FATAL_ERROR "Invalid execution of this script detected.")
endif()

if(NOT EXISTS "${JAFG_MOTOR_EXECUTABLE}")
    message(STATUS "Motor at [${JAFG_MOTOR_EXECUTABLE}] is not valid.")
    include(Programs/RecompileMotor.cmake)
    RecompileMotor()
else()
    message(STATUS "Motor at [${JAFG_MOTOR_EXECUTABLE}] is valid. Skipping recompilation.")
endif()
if(NOT EXISTS "${JAFG_MOTOR_EXECUTABLE}")
    message(FATAL_ERROR "Motor executable [${JAFG_MOTOR_EXECUTABLE}] could not be found or compilation process failed silently.")
endif()

function(PrebuildModuleWithMotor module_path)
    message(STATUS "Running pre-build motor for module [${module_path}] ...")
    execute_process(
        COMMAND             ${JAFG_MOTOR_EXECUTABLE}
                                --Verbose
                                --PreBuild
                                --Module ${module_path}
                                --Platform ${JAFG_TARGET_PLATFORM}
                                --Architecture ${JAFG_TARGET_ARCHITECTURE}
                                --Target ${JAFG_TARGET_TYPE}
                                --Configuration ${JAFG_TARGET_CONFIG}
                                --Kind unknown
        WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
        OUTPUT_VARIABLE     stdout
        ERROR_VARIABLE      stderr
        RESULT_VARIABLE     result
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        if(stdout AND NOT stdout STREQUAL "")
            message(STATUS "[${module_path}]: Motor stdout:\n${stdout}")
        endif()
        if(stderr AND NOT stderr STREQUAL "")
            message(STATUS "[${module_path}]: Motor stderr:\n${stderr}")
        endif()

        if(result)
            message(FATAL_ERROR "[${module_path}]: Motor quit with exit code [${result}].")
        else()
            message(STATUS "[${module_path}]: Motor quit with exit code [${result}].")
        endif()
endfunction()
