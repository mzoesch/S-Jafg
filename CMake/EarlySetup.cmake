# Copyright mzoesch. All rights reserved.

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "ENGINE_ROOT: ${JAFG_ENGINE_ROOT}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

###############################################################################
# Ensure the compiler.
###############################################################################

if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
    string(REGEX MATCH ".*clang\\+\\+.*" REGREX_MATCHED ${CMAKE_CXX_COMPILER})
    if(NOT REGREX_MATCHED)
        message(FATAL_ERROR "C [${CMAKE_C_COMPILER}] and C++ [${CMAKE_CXX_COMPILER}] compiler are not from the Clang toolchain.")
    endif()
elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
    if(NOT CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        message(FATAL_ERROR "C [${CMAKE_C_COMPILER}] and C++ [${CMAKE_CXX_COMPILER}] compiler are not from the MSVC toolchain.")
    endif()
elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WASM)
    string(REGEX MATCH ".*em\\+\\+.*" REGREX_MATCHED ${CMAKE_CXX_COMPILER})
    if(NOT REGREX_MATCHED)
        message(FATAL_ERROR "C and CXX compiler [EM] is required for this platform [${JAFG_TARGET_PLATFORM}].")
    endif()
else()
    message(FATAL_ERROR "Missing implementation here for JAFG_TARGET_PLATFORM [${JAFG_TARGET_PLATFORM}].")
endif()

###############################################################################
# Fetch submodules.
###############################################################################

function(UpdateSubmodules)
    if(NOT EXISTS "${JAFG_ENGINE_ROOT}/.git")
        return()
    endif()
    if(NOT EXISTS "${JAFG_ENGINE_ROOT}/.gitmodules")
        return()
    endif()

    execute_process(
        COMMAND             git submodule update --init --recursive
        WORKING_DIRECTORY   ${JAFG_ENGINE_ROOT}
        OUTPUT_VARIABLE     stdout
        ERROR_VARIABLE      stderr
        RESULT_VARIABLE     result
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )

    if(stdout AND NOT stdout STREQUAL "")
        message(STATUS "Git submodule fetcher stdout:\n${stdout}")
    endif()
    if(stderr AND NOT stderr STREQUAL "")
        message(STATUS "Git submodule fetcher stderr:\n${stderr}")
    endif()

    if(result)
        message(FATAL_ERROR "Git submodule fetcher quit with exit code [${result}].")
    else()
        message(STATUS "Git submodule fetcher quit with exit code [${result}].")
    endif()
endfunction()

UpdateSubmodules()

###############################################################################
# The motor of Jafg.
###############################################################################

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

###############################################################################
# Misc.
###############################################################################

include(CMake/ThrowOnInSourceBuild.cmake)
include(CMake/ThrowOnMultiConfiguration.cmake)
include(CMake/ModuleFlags.cmake)

# CMake extensions.
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(FetchContent)
include(CMake/Json.cmake)

if(NOT EXISTS "${JAFG_ENGINE_ROOT}/jafg.jafgworkspace")
    message(FATAL_ERROR "Jafg engine root directory was not found. Falsely evaluated to: [${JAFG_ENGINE_ROOT}].")
endif()

function(DisableAllWarningsForTarget target_name)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${target_name} PRIVATE
                -w              # Suppress all warnings
                )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target_name} PRIVATE
                /W              # Suppress all warnings
                )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()
endfunction()

###############################################################################
# Global flags. Eww
###############################################################################

#
# The performance cost is neglectable for the insane amount of benefits it brings.
# So just enable globally (eww global settings; god redeem us for our global preferences).
#
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

if(LAL_DO_SANITIZED_BUILD)
    message(STATUS "Enabling sanitizers for the current build.")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_options(
            -fsanitize=address                  # address:                 Detects use-after-free, heap buffer overflows, etc.
            -fsanitize=undefined                # undefined:               Detects undefined behavior, like null deref, signed int overflow, etc.
            -fsanitize=leak                     # leak:                    Detects memory leaks.
            -fsanitize=signed-integer-overflow  # signed-integer-overflow: Detects signed integer overflows.
            -fsanitize=integer                  # integer:                 Detects integer overflows, division by zero, etc.
            -fno-omit-frame-pointer             # Keeps frame pointers for better stack traces.
            -fstack-protector-strong            # Adds stack canaries to detect buffer overflows.
            -ftrivial-auto-var-init=pattern     # Fill uninitialized local variables with a pattern.
            )
        add_link_options(
            -fsanitize=address      # @see add_compile_options above.
            -fsanitize=undefined
            -fsanitize=leak
            -fsanitize=signed-integer-overflow
            -fsanitize=integer
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()
endif()

if(LAL_DO_HARDEN_BUILD)
    message(STATUS "Enabling hardening flags for the current build.")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_link_options(
            -Wl,-z,relro,-z,now,-z,noexecstack  # relro:       Enable RELRO (Read-Only Relocations) and NX (No eXecute) stack.
                                                # now:         Make the RELRO section read-only immediately after loading.
                                                # noexecstack: Mark the stack as non-executable.
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()
endif()

###############################################################################
# Motor.
###############################################################################
if(NOT DEFINED JAFG_MOTOR_EXECUTABLE)
    message(FATAL_ERROR "Invalid execution of this script detected.")
endif()

if(NOT EXISTS "${JAFG_MOTOR_EXECUTABLE}")
    message(STATUS "Motor at [${JAFG_MOTOR_EXECUTABLE}] is not valid.")
    include(CMake/RecompileMotor.cmake)
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
