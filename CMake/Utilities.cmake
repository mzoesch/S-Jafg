# Copyright mzoesch. All rights reserved.

function(retrieve_file_content p_file_path p_found p_content)
    if(NOT EXISTS "${p_file_path}")
        set(${p_found} FALSE PARENT_SCOPE)
        set(${p_content} "" PARENT_SCOPE)
        return()
    endif()

    file(READ "${p_file_path}" _file_content)
    string(STRIP "${_file_content}" _stripped_content)

    set(${p_found} TRUE PARENT_SCOPE)

    if(NOT _stripped_content)
        set(${p_content} "" PARENT_SCOPE)
    else()
        set(${p_content} "${_stripped_content}" PARENT_SCOPE)
    endif()
endfunction()

function(retrieve_file_content_no_fail p_file_path p_content)
    retrieve_file_content("${p_file_path}" _found _content)
    if(NOT _found)
        message(FATAL_ERROR "[${p_file_path}]: No such file.")
    endif()
    set(${p_content} "${_content}" PARENT_SCOPE)
endfunction()

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
        message(WARNING "Git submodule fetcher quit with exit code [${result}].")
    else()
        message(STATUS "Git submodule fetcher quit with exit code [${result}].")
    endif()
endfunction()

function(DisableAllWarningsForTarget target_name)
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${target_name} PRIVATE
            -w              # Suppress all warnings
            )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target_name} PRIVATE
            /w              # Suppress all warnings
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()
endfunction()

function(SanitizeThisBuildGlobally)
    message(STATUS "Enabling sanitizers for the current build.")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
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
endfunction()

function(HardenThisBuildGlobally)
    message(STATUS "Enabling hardening flags for the current build.")
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        add_link_options(
            -Wl,-z,relro,-z,now,-z,noexecstack  # relro:       Enable RELRO (Read-Only Relocations) and NX (No eXecute) stack.
            # now:         Make the RELRO section read-only immediately after loading.
            # noexecstack: Mark the stack as non-executable.
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()
endfunction()

#
# An option, that, if not defined in CMake, will be automatically resolved in C++ code to a default value
# based of the target platform, target type and target configuration.
#
macro(unspecified_flag
    flag_name
    flag_description
    )
    if(NOT DEFINED CACHE{${flag_name}})
        set(
            ${flag_name}
            "Unspecified"
            CACHE STRING
            "${flag_description}"
            )
    endif()
    set_property(CACHE ${flag_name} PROPERTY STRINGS "Unspecified" "1" "0")
endmacro()

function(jafg_hint_debug_symbols p_debug_symbols)
    set(${p_debug_symbols} FALSE PARENT_SCOPE)
    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        set(${p_debug_symbols} TRUE PARENT_SCOPE)
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT)
        set(${p_debug_symbols} TRUE PARENT_SCOPE)
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        if(JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING)
            set(${p_debug_symbols} TRUE PARENT_SCOPE)
        endif()
    else()
        message(FATAL_ERROR "Unknown JAFG_TARGET_CONFIG [${JAFG_TARGET_CONFIG}].")
    endif()
endfunction()
