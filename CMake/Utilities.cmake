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

#
# A function that allow pulling content from an arbitrary file server that provides a manifest, as specified
# in Programs/GenerateManifest.sh.
#
function(HandleContentDependencies
    p_local_content_root        # The local target content dir; e.g.: Plugins/MyPlugin/Content
    p_remote_content_server     # The remote content server URL; e.g.: https://myserver.com/myplugin/v1.0.0
    )
    set(lfs_file_path "${p_local_content_root}/.lfs")
    if(NOT EXISTS "${lfs_file_path}")
        message(FATAL_ERROR "[${lfs_file_path}]: No such file.")
    endif()
    file(STRINGS "${lfs_file_path}" lfs_content)

    if(DEFINED ENV{JAFG_UPGRADE_CONTENT})
        set(remote_manifest_url "${p_remote_content_server}/manifest")
        set(remote_manifest_path "${CMAKE_CURRENT_BINARY_DIR}/Content/manifest")
        message(STATUS "Downloading content manifest from [${remote_manifest_url}] to [${remote_manifest_path}] ...")
        file(DOWNLOAD
            "${remote_manifest_url}"
            "${remote_manifest_path}"
            SHOW_PROGRESS
            STATUS download_status
            )
        list(GET download_status 0 download_result)
        if(NOT download_result EQUAL 0)
            message(FATAL_ERROR "Failed to download content manifest from [${remote_manifest_url}].")
        endif()
        file(STRINGS "${remote_manifest_path}" remote_manifest_files)
        foreach(line IN LISTS remote_manifest_files)
            message(VERBOSE "REMOTE: ${line}")
        endforeach()
    endif()

    function(jafg_get_manifest_hash OUT_HASH FILE_PATH)
        set(${OUT_HASH} "NONE" PARENT_SCOPE)
        if(NOT DEFINED remote_manifest_files)
            message(FATAL_ERROR "remote_manifest_files is not defined.")
        endif()
        foreach(line IN LISTS remote_manifest_files)
            string(REGEX MATCH "^([^ ]+)[ ]+(.+)$" MATCHED "${line}")
            if(MATCHED)
                set(MANIFEST_HASH "${CMAKE_MATCH_1}")
                set(MANIFEST_PATH "${CMAKE_MATCH_2}")
                if(MANIFEST_PATH STREQUAL "${FILE_PATH}")
                    set(${OUT_HASH} "${MANIFEST_HASH}" PARENT_SCOPE)
                    return()
                endif()
            endif()
        endforeach()
    endfunction()

    foreach(line IN LISTS lfs_content)
        if(line STREQUAL "") # Empty line
            continue()
        endif()
        if(line MATCHES "^\\s*#.*") # Comment line.
            continue()
        endif()
        if(NOT (line MATCHES "^[a-zA-Z\\d\/\.\,\-\_]+$"))
            message(FATAL_ERROR "[${lfs_file_path}]: Invalid line [${line}].")
        endif()
        set(_content_file "${p_local_content_root}/${line}")
        set(_content_url "${p_remote_content_server}/${line}")
        if((NOT EXISTS "${_content_file}") OR DEFINED ENV{JAFG_FORCE_CONTENT_UPDATE})
            message(STATUS "Downloading [${_content_url}] to [${_content_file}] ...")
            file(DOWNLOAD
                "${_content_url}"
                "${_content_file}"
                SHOW_PROGRESS
                STATUS download_status
                )
            list(GET download_status 0 download_result)
            if(NOT download_result EQUAL 0)
                message(FATAL_ERROR "[${_content_file}]: Failed to download content file from [${_content_url}].")
            endif()
        elseif(DEFINED ENV{JAFG_UPGRADE_CONTENT})
            if(NOT DEFINED remote_manifest_files)
                message(FATAL_ERROR "remote_manifest_files is not defined.")
            endif()
            file(SHA256 "${_content_file}" _hash)
            file(RELATIVE_PATH _relative_content_file "${p_local_content_root}" "${_content_file}")
            jafg_get_manifest_hash(_remote_hash "${_relative_content_file}")
            if(_hash STREQUAL _remote_hash)
                message(STATUS "[${_relative_content_file}]: Up to date.")
            else()
                message(STATUS "[${_relative_content_file}]: Outdated. Local(${_hash}) != Remote(${_remote_hash}). Downloading...")
                file(DOWNLOAD
                    "${_content_url}"
                    "${_content_file}"
                    SHOW_PROGRESS
                    STATUS download_status
                    )
                list(GET download_status 0 download_result)
                if(NOT download_result EQUAL 0)
                    message(FATAL_ERROR "[${_relative_content_file}]: Failed to download content file from [${_content_url}].")
                endif()
            endif()
        else()
            message(STATUS "[${_content_file}]: Exists; skipped remote sha check.")
        endif()
    endforeach()
endfunction()
