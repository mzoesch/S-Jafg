# Copyright mzoesch. All rights reserved.

if(NOT EXISTS "${JAFG_MOTOR_TOML}")
    message(FATAL_ERROR "Cargo.toml not found in ${JAFG_MOTOR_DIR}. Please ensure you are in the correct directory.")
endif()

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
        message(FATAL_ERROR "Motor compiled with exit code [${result}].")
    else()
        message(STATUS "Motor compiled with exit code [${result}].")
    endif()
endfunction()
