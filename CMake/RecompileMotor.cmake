# Copyright mzoesch. All rights reserved.

if(NOT EXISTS "${JAFG_MOTOR_TOML}")
    message(FATAL_ERROR "Cargo.toml not found in ${JAFG_MOTOR_DIR}. Please ensure you are in the correct directory.")
endif()

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
