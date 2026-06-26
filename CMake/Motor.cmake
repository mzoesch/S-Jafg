# Copyright mzoesch. All rights reserved.

function(DownloadMotor)
    set(_full_download_url "${JAFG_MOTOR_PRE_BUILD_BINARY_URL_ENDPOINT}${JAFG_MOTOR_PRE_BUILD_BINARY_URL_ROUTE}")
    message(STATUS "Downloading Motor from [${_full_download_url}] to [${JAFG_MOTOR_EXECUTABLE}] ...")

    if(JAFG_MOTOR_TARGET_CONFIG STREQUAL "debug")
        message(FATAL_ERROR "Debug builds of Motor are not supported for download. Please recompile Motor instead. You may download release builds with -DJAFG_MOTOR_TARGET_CONFIG=release.")
    elseif(JAFG_MOTOR_TARGET_CONFIG STREQUAL "release")
    else()
        message(FATAL_ERROR "Failed to detect motor target config [${JAFG_MOTOR_TARGET_CONFIG}]. Must be either 'debug' or 'release'.")
    endif()

    if(EXISTS "${JAFG_MOTOR_EXECUTABLE}")
        message(WARNING "Motor executable already exists at [${JAFG_MOTOR_EXECUTABLE}]. Skipping download.")
        return()
    endif()

    get_filename_component(_motor_executable_dir "${JAFG_MOTOR_EXECUTABLE}" DIRECTORY)
    if(NOT EXISTS "${_motor_executable_dir}")
        file(MAKE_DIRECTORY "${_motor_executable_dir}")
        message(STATUS "Created directory [${_motor_executable_dir}].")
    else()
        message(STATUS "Motor executable directory [${_motor_executable_dir}] already exists. Skipping parent directory creation.")
    endif()

    execute_process(
        COMMAND             curl -L -o "${JAFG_MOTOR_EXECUTABLE}" "${_full_download_url}"
        RESULT_VARIABLE     result
        OUTPUT_VARIABLE     stdout
        ERROR_VARIABLE      stderr
        OUTPUT_STRIP_TRAILING_WHITESPACE
        )
    if(stdout AND NOT stdout STREQUAL "")
        message(STATUS "Motor download stdout:\n${stdout}")
    endif()
    if(stderr AND NOT stderr STREQUAL "")
        message(STATUS "Motor download error stderr:\n${stderr}")
    endif()

    if(result)
        message(FATAL_ERROR "Failed to download Motor from [${_full_download_url}]. Exit code: [${result}].")
    endif()

    message(STATUS "Motor downloaded successfully to [${JAFG_MOTOR_EXECUTABLE}].")

    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        execute_process(
            COMMAND             chmod +x "${JAFG_MOTOR_EXECUTABLE}"
            RESULT_VARIABLE     result
            OUTPUT_VARIABLE     stdout
            ERROR_VARIABLE      stderr
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
        if(stdout AND NOT stdout STREQUAL "")
            message(STATUS "Motor executable permission change stdout:\n${stdout}")
        endif()
        if(stderr AND NOT stderr STREQUAL "")
            message(STATUS "Motor executable permission change error stderr:\n${stderr}")
        endif()

        if(result)
            message(FATAL_ERROR "Failed to set executable permissions for Motor at [${JAFG_MOTOR_EXECUTABLE}]. Exit code: [${result}].")
        else()
            message(STATUS "Motor executable permissions set successfully.")
        endif()
    endif()
endfunction()

function(RecompileMotor)
    message(STATUS "Recompiling Motor in [${JAFG_MOTOR_DIR}] ...")

    if(JAFG_MOTOR_TARGET_CONFIG STREQUAL "debug")
        execute_process(
            COMMAND             cargo build --verbose
            WORKING_DIRECTORY   "${JAFG_MOTOR_DIR}"
            OUTPUT_VARIABLE     stdout
            ERROR_VARIABLE      stderr
            RESULT_VARIABLE     result
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    elseif(JAFG_MOTOR_TARGET_CONFIG STREQUAL "release")
        execute_process(
            COMMAND             cargo build --release --verbose
            WORKING_DIRECTORY   "${JAFG_MOTOR_DIR}"
            OUTPUT_VARIABLE     stdout
            ERROR_VARIABLE      stderr
            RESULT_VARIABLE     result
            OUTPUT_STRIP_TRAILING_WHITESPACE
            )
    else()
        message(FATAL_ERROR "Invalid JAFG_MOTOR_TARGET_CONFIG [${JAFG_MOTOR_TARGET_CONFIG}]. Must be either 'debug' or 'release'.")
    endif()

    if(stdout AND NOT stdout STREQUAL "")
        message(STATUS "Motor compilation stdout:\n${stdout}")
    endif()
    if(stderr AND NOT stderr STREQUAL "")
        message(STATUS "Motor compilation error stderr:\n${stderr}")
    endif()

    if(result)
        message(WARNING "Motor compiled with exit code [${result}]. Attempting to download pre-build Motor instead.")
        DownloadMotor()
    else()
        message(STATUS "Motor compiled with exit code [${result}].")
    endif()
endfunction()

function(PrebuildModuleWithMotor module_path)
    message(STATUS "Running pre-build motor for module [${module_path}] in binary dir [${CMAKE_CURRENT_BINARY_DIR}]...")
    execute_process(
        COMMAND             ${JAFG_MOTOR_EXECUTABLE}
        --Verbose
        --BinaryDir ${CMAKE_CURRENT_BINARY_DIR}
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
        string(CONCAT _invoked_command
            "${JAFG_MOTOR_EXECUTABLE} "
            "--Verbose "
            "--Module ${module_path} "
            "--Platform ${JAFG_TARGET_PLATFORM} "
            "--Architecture ${JAFG_TARGET_ARCHITECTURE} "
            "--Target ${JAFG_TARGET_TYPE} "
            "--Configuration ${JAFG_TARGET_CONFIG} "
            "--Kind unknown"
            )
        message(FATAL_ERROR "[${module_path}]: Motor invoked command [${_invoked_command}] failed with exit code [${result}].")
    else()
        message(STATUS "[${module_path}]: Motor quit with exit code [${result}].")
    endif()
endfunction()

if(NOT DEFINED JAFG_MOTOR_PRE_BUILD_BINARY_URL_ENDPOINT)
    set(JAFG_MOTOR_PRE_BUILD_BINARY_URL_ENDPOINT "https://api.zoeschinger.com/hooks/jafg")
endif()
if(NOT DEFINED JAFG_MOTOR_LINUX_PRE_BUILD_BINARY_URL_ROUTE)
    set(JAFG_MOTOR_LINUX_PRE_BUILD_BINARY_URL_ROUTE "/static/Motor/target/x86_64-unknown-linux-gnu/release/motor")
endif()
if(NOT DEFINED JAFG_MOTOR_WINDOWS_PRE_BUILD_BINARY_URL_ROUTE)
    set(JAFG_MOTOR_WINDOWS_PRE_BUILD_BINARY_URL_ROUTE "/static/Motor/target/x86_64-pc-windows-gnu/release/motor.exe")
endif()

set(JAFG_MOTOR_TARGET_CONFIG "release" CACHE STRING "Target config for motor")
set_property(CACHE JAFG_MOTOR_TARGET_CONFIG PROPERTY STRINGS debug release)

set(JAFG_MOTOR_DIR "${JAFG_ENGINE_ROOT}/Motor")

if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
    set(JAFG_MOTOR_EXECUTABLE "${JAFG_MOTOR_DIR}/target/${JAFG_MOTOR_TARGET_CONFIG}/motor")
    set(JAFG_MOTOR_PRE_BUILD_BINARY_URL_ROUTE "${JAFG_MOTOR_LINUX_PRE_BUILD_BINARY_URL_ROUTE}")
elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
    set(JAFG_MOTOR_EXECUTABLE "${JAFG_MOTOR_DIR}/target/${JAFG_MOTOR_TARGET_CONFIG}/motor.exe")
    set(JAFG_MOTOR_PRE_BUILD_BINARY_URL_ROUTE "${JAFG_MOTOR_WINDOWS_PRE_BUILD_BINARY_URL_ROUTE}")
endif()
if(NOT DEFINED JAFG_MOTOR_EXECUTABLE)
    message(FATAL_ERROR "JAFG_MOTOR_EXECUTABLE is not defined. Please define it with -DJAFG_MOTOR_EXECUTABLE=<path to motor executable>.")
endif()
message(STATUS "JAFG_MOTOR_EXECUTABLE: ${JAFG_MOTOR_EXECUTABLE}")

set(JAFG_MOTOR_TOML "${JAFG_MOTOR_DIR}/Cargo.toml")
if(NOT EXISTS "${JAFG_MOTOR_TOML}")
    message(FATAL_ERROR "[${JAFG_MOTOR_TOML}]: No such file.")
endif()
