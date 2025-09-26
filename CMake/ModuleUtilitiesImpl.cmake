# Copyright mzoesch. All rights reserved.

include(CMake/RetrieveFileContent.cmake)

macro(_jafg_add_module
    module_type
    )
    get_filename_component(module_name "${CMAKE_CURRENT_LIST_DIR}" NAME)
    _jafg_add_module_impl(${module_name} ${module_type} "${CMAKE_CURRENT_LIST_DIR}")
    set(_scoped_module_name "${module_name}")
endmacro()

macro(_jafg_add_flag_if_specified
    flag
    cpp_name
    )
    if(NOT ${flag} STREQUAL "Unspecified")
        target_compile_definitions(${module_name} PRIVATE
            ${cpp_name}=${flag}
            )
    endif()
endmacro()

function(_jafg_add_module_impl
    module_name
    module_type
    module_dir
    )
    file(RELATIVE_PATH module_rel_dir "${JAFG_ENGINE_ROOT}" "${module_dir}")

    if(${module_name} STREQUAL "")
        message(FATAL_ERROR "Module name is not set.")
    endif()

    if(
        (NOT ${module_type} STREQUAL JAFG_MODULE_TYPE_STATIC) AND
        (NOT ${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED) AND
        (NOT ${module_type} STREQUAL JAFG_MODULE_TYPE_LAUNCH) AND
        (NOT ${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
        )
        message(FATAL_ERROR "Invalid module type [${module_type}].")
    endif()

    get_property(is_already_added_path GLOBAL PROPERTY "_jafg_added_modules_map_path_${module_rel_dir}")
    if(DEFINED is_already_added_path)
        message(FATAL_ERROR "Module path [${module_rel_dir}] is already added.")
    endif()
    get_property(is_already_added_name GLOBAL PROPERTY "_jafg_added_modules_map_name_${module_name}")
    if(DEFINED is_already_added_name)
        message(FATAL_ERROR "Module name [${module_name}] is already added.")
    endif()

    set_property(GLOBAL PROPERTY "_jafg_added_modules_map_path_${module_rel_dir}" "${module_type}")
    set_property(GLOBAL PROPERTY "_jafg_added_modules_map_name_${module_name}" "${module_rel_dir}")

    get_property(is_found GLOBAL PROPERTY "_jafg_added_modules_map_path_${module_rel_dir}")
    if(NOT DEFINED is_found)
        message(FATAL_ERROR "Failed to add [${module_rel_dir}] to global path map registry as [_jafg_added_modules_map_path_${module_rel_dir}].")
    endif()
    get_property(is_found GLOBAL PROPERTY "_jafg_added_modules_map_name_${module_name}")
    if(NOT DEFINED is_found)
        message(FATAL_ERROR "Failed to add [${module_name}] to global name map registry as _jafg_added_modules_map_name_${module_name}].")
    endif()

    message(STATUS "[${module_rel_dir}]: Added from [${module_dir}].")

    string(TOUPPER "${module_name}" module_name_upper)

    set(module_int_dir "${JAFG_ENGINE_ROOT}/Intermediates/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}")
    PrebuildModuleWithMotor(${module_rel_dir})
    if(NOT EXISTS "${module_int_dir}")
        message(FATAL_ERROR "Module intermediate directory [${module_int_dir}] is not valid. Motor failed silently.")
    endif()

    file(GLOB_RECURSE src_files
        "${module_dir}/Source/*.h"
        "${module_dir}/Source/*.hpp"
        "${module_dir}/Source/*.c"
        "${module_dir}/Source/*.cpp"
        "${module_int_dir}/gt/*.cpp"
        )

    file(GLOB_RECURSE src_files_c
        "${module_dir}/Source/*.c"
        )

    if(${module_type} STREQUAL JAFG_MODULE_TYPE_LAUNCH)
        add_executable(${module_name} ${src_files})
        set(motor_module_type "launch")
        target_compile_definitions(${module_name} PRIVATE
            ${module_name_upper}_API=
            )
    elseif(${module_type} STREQUAL JAFG_MODULE_TYPE_STATIC)
        add_library(${module_name} STATIC ${src_files})
        set(motor_module_type "static")
        target_compile_definitions(${module_name} PUBLIC
            ${module_name_upper}_API=
            )
    elseif((${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED) OR (${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN))
        add_library(${module_name} SHARED ${src_files})
        if(${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED)
            set(motor_module_type "shared")
        elseif(${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
            set(motor_module_type "plugin")
        else()
            message(FATAL_ERROR "Missing implementation for type [${module_type}].")
        endif()
        target_compile_definitions(${module_name} PRIVATE
            ${module_name_upper}_API=LAL_PLATFORM_CALLSPEC_OUT
            )
        target_compile_definitions(${module_name} INTERFACE
            ${module_name_upper}_API=LAL_PLATFORM_CALLSPEC_IN
            )
    else()
        message(FATAL_ERROR "Invalid module type [${module_type}].")
    endif()

    if(NOT DEFINED motor_module_type)
        message(FATAL_ERROR "Could not resolve module motor type for [${module_rel_dir}].")
    endif()

    foreach(src_file_c ${src_files_c})
        set_source_files_properties(${src_file} PROPERTIES LANGUAGE C)
        set_source_files_properties(${src_file} PROPERTIES SKIP_PRECOMPILE_HEADERS ON)
    endforeach()

    target_include_directories(${module_name} PUBLIC
        "${module_dir}/Source/Public"
        "${module_int_dir}/gh"
        )
    target_include_directories(${module_name} PRIVATE
        "${JAFG_ENGINE_ROOT}"
        "${module_dir}/Source/Internal"
        "${JAFG_ENGINE_ROOT}/Intermediates/${module_rel_dir}"
        )

    set_target_properties(${module_name} PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${JAFG_ENGINE_ROOT}/Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}"
        )

    set(pch_file "${module_dir}/Source/Internal/Module.pch")
    if(EXISTS "${pch_file}")
        target_precompile_headers(${module_name} PRIVATE
            "${pch_file}"
            )
    endif()

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
        target_compile_definitions(${module_name} PRIVATE
            PLATFORM_LINUX=1
            )
    elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
        target_compile_definitions(${module_name} PRIVATE
            PLATFORM_WINDOWS=1
            )
        if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_definitions(${module_name} PRIVATE
                LAL_PLATFORM_WINDOWS_WITH_MSVC=1
                )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            target_compile_definitions(${module_name} PRIVATE
                LAL_PLATFORM_WINDOWS_WITH_GCC=1
            )
        else()
            message(FATAL_ERROR "Compiler not supported for Windows: [${CMAKE_CXX_COMPILER_ID}].")
        endif()
    elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WASM)
        target_compile_definitions(${module_name} PRIVATE
            PLATFORM_WASM=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_PLATFORM [${JAFG_TARGET_PLATFORM}].")
    endif()

    target_compile_definitions(${module_name} PRIVATE
        PRIVATE_LAL_CPLUSPLUS=${_private_jafg_min_cplusplus}
        )

    if(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_CLIENT)
        target_compile_definitions(${module_name} PRIVATE
            AS_CLIENT=1
            )
    elseif(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_DAEMON)
        target_compile_definitions(${module_name} PRIVATE
            AS_DAEMON=1
            )
    elseif(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_TESTUNIT)
        target_compile_definitions(${module_name} PRIVATE
            AS_CLIENT=1
            WITH_TESTS=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_TYPE [${JAFG_TARGET_TYPE}].")
    endif()

    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        target_compile_definitions(${module_name} PRIVATE
            IN_DEBUG=1
            )
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT)
        target_compile_definitions(${module_name} PRIVATE
            IN_DEVELOPMENT=1
            )
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        target_compile_definitions(${module_name} PRIVATE
            IN_SHIPPING=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_CONFIG [${JAFG_TARGET_CONFIG}].")
    endif()

    ###############################################################################
    # Compiler flags
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(${module_name} PRIVATE
            -fno-rtti                   # No RTTI.
            # This is the default for clang, so just ignore it for now.
            # -stdlib=libstdc++         # Use libstdc++ instead of LLVM's libc++.
            -fno-exceptions             # No exceptions.
            -fno-common                 # Forces global variables to be defined in exactly one object file.
            -fvisibility=hidden         # Hides all symbols by default.
            -Wall -Wextra -Wpedantic    # Enable many warnings.
            # -Weverything <-- To much for now.
            )
        target_link_options(${module_name} PRIVATE
            -rdynamic
            )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${module_name} PRIVATE
            /nologo                     # Annoying as shit.
            /permissive-                # Biggest mistake by Microsoft.
            /Zc:__cplusplus             # Why the fuck microsoft?
            /GR-                        # No RTTI.
            /MP                         # Multiple processors.
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()

    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${module_name} PRIVATE
                -g              # Debug symbols.
                -O0             # Prevent optimizations.
                )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(${module_name} PRIVATE
                /Zi             # PDB debug symbols.
                /Od             # Prevent optimizations.
                )
        else()
            message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
        endif()
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${module_name} PRIVATE
                -g              # Debug symbols.
                -O2             # Tsundere optimizations.
                )
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            target_compile_options(${module_name} PRIVATE
                /Zi             # PDB debug symbols.
                /O2             # Pessimistic optimizations.
                )
        else()
            message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
        endif()
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            if(LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING)
                target_compile_options(${module_name} PRIVATE
                    -g              # Debug symbols.
                    -O3             # Aggressive optimizations (Prioritize speed and fuck security).
                    )
            else()
                target_compile_options(${module_name} PRIVATE
                    -g0             # No debug symbols.
                    -O3             # Aggressive optimizations.
                    )
            endif()
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            if(LAL_DO_DEBUG_SYMBOLS_IN_SHIPPING)
                target_compile_options(${module_name} PRIVATE
                    /Zi             # PDB debug symbols.
                    /Ox             # Aggressive optimizations.
                    /GL             # Whole program optimization.
                    )
            else()
                target_compile_options(${module_name} PRIVATE
                    /Od             # No debug symbols.
                    /Ox             # Aggressive optimizations.
                    /GL             # Whole program optimization.
                    )
            endif()
        else()
            message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
        endif()
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_CONFIG [${JAFG_TARGET_CONFIG}].")
    endif()

    set(b_LAL_DO_COMPILER_DIAGNOSTIC_SETUP $<IF:$<BOOL:LAL_DO_COMPILER_DIAGNOSTIC_SETUP>,1,0>)

    target_compile_definitions(${module_name} PRIVATE
        LAL_DO_COMPILER_DIAGNOSTIC_SETUP=${b_LAL_DO_COMPILER_DIAGNOSTIC_SETUP}
        )

    _jafg_add_flag_if_specified(${LAL_DO_ENABLE_SHIPPING_WARNINGS}           "LAL_DO_ENABLE_SHIPPING_WARNINGS")

    _jafg_add_flag_if_specified(${LAL_LOG_DEFAULT_VERBOSITY}                 "LAL_LOG_DEFAULT_VERBOSITY")
    _jafg_add_flag_if_specified(${LAL_LOG_ENABLE_TRACE}                      "LAL_LOG_ENABLE_TRACE")
    _jafg_add_flag_if_specified(${LAL_LOG_ENABLE_VERBOSE}                    "LAL_LOG_ENABLE_VERBOSE")
    _jafg_add_flag_if_specified(${LAL_LOG_ENABLE_INFO}                       "LAL_LOG_ENABLE_INFO")
    _jafg_add_flag_if_specified(${LAL_LOG_ENABLE_WARNING}                    "LAL_LOG_ENABLE_WARNING")
    _jafg_add_flag_if_specified(${LAL_LOG_ENABLE_ERROR}                      "LAL_LOG_ENABLE_ERROR")
    _jafg_add_flag_if_specified(${LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER}      "LAL_LOG_DO_SCOPED_TIME_TASK_MEASURER")
    _jafg_add_flag_if_specified(${JAFG_FORCE_LOG_FLUSH_INTERVAL}             "JAFG_FORCE_LOG_FLUSH_INTERVAL")
    _jafg_add_flag_if_specified(${JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES}        "JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES")

    _jafg_add_flag_if_specified(${LAL_CHECK_CONTAINER_BOUNDS}                "LAL_CHECK_CONTAINER_BOUNDS")
    _jafg_add_flag_if_specified(${LAL_CHECK_STRING_VALIDITY}                 "LAL_CHECK_STRING_VALIDITY")
    _jafg_add_flag_if_specified(${LAL_CHECK_ARRAY}                           "LAL_CHECK_ARRAY")
    # ~Compiler flags
    ###############################################################################

    add_custom_target(zzz_${module_name}_PRE_BUILD
        COMMAND ${JAFG_MOTOR_EXECUTABLE}
            --Module ${module_rel_dir}
            --Platform ${JAFG_TARGET_PLATFORM}
            --Architecture ${JAFG_TARGET_ARCHITECTURE}
            --Target ${JAFG_TARGET_TYPE}
            --Configuration ${JAFG_TARGET_CONFIG}
            --Kind ${motor_module_type}
        )
    add_dependencies(${module_name} zzz_${module_name}_PRE_BUILD)

    ###############################################################################
    # Human readable plugin info file
    if(${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
        set(_target_root_plugin_jafg "${JAFG_ENGINE_ROOT}/Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}/.jafg.root.plugin")
        if(NOT EXISTS "${_target_root_plugin_jafg}")
            retrieve_file_content_no_fail("${JAFG_ENGINE_ROOT}/${module_rel_dir}/Config/.ver" _target_ver)

            get_target_property(loc_type ${module_name} TYPE)
            if(loc_type STREQUAL "STATIC_LIBRARY" OR loc_type STREQUAL "SHARED_LIBRARY")
                set(prefix "${CMAKE_SHARED_LIBRARY_PREFIX}")
                set(suffix "${CMAKE_SHARED_LIBRARY_SUFFIX}")
            elseif(loc_type STREQUAL "MODULE_LIBRARY")
                set(prefix "${CMAKE_SHARED_MODULE_PREFIX}")
                set(suffix "${CMAKE_SHARED_MODULE_SUFFIX}")
            elseif(loc_type STREQUAL "EXECUTABLE")
                set(prefix "")
                set(suffix "${CMAKE_EXECUTABLE_SUFFIX}")
            endif()

            if(DEFINED this_plugin_identifier)
                set(_this_plugin_identifier "${this_plugin_identifier}")
                unset(this_plugin_identifier PARENT_SCOPE)
            else()
                set(_this_plugin_identifier "${module_name}")
            endif()
            if(DEFINED this_plugin_friendly_name)
                set(_this_plugin_friendly_name "${this_plugin_friendly_name}")
                unset(this_plugin_friendly_name PARENT_SCOPE)
            else()
                set(_this_plugin_friendly_name "${module_name}")
            endif()

            set(_target_root_plugin_jafg_content
"{
    \"Version\": \"${_target_ver}\",
    \"Identifier\": \"${this_plugin_identifier}\",
    \"NativeIdentifier\": \"${module_name}\",
    \"FriendlyName\": \"${_this_plugin_friendly_name}\",
    \"Bin\": \"Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}/${prefix}${module_name}${suffix}\"
}
")
            file(MAKE_DIRECTORY "${JAFG_ENGINE_ROOT}/Binaries/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}")
            file(WRITE "${_target_root_plugin_jafg}" "${_target_root_plugin_jafg_content}")
            message(STATUS "[${module_rel_dir}]: Created plugin info file at [${_target_root_plugin_jafg}].")
        endif()
    endif()
    # ~Human readable plugin info file
    ###############################################################################
endfunction()

macro(_jafg_add_dependency
    dep_path
    visibility
    )
    function(_scoped_jafg_add_dependency dep_path)
        include("${JAFG_ENGINE_ROOT}/${dep_path}/CMakeLists.txt")
    endfunction()
    _scoped_jafg_add_dependency("${dep_path}")

    _jafg_add_dependency_impl(${_scoped_module_name} ${dep_path} ${visibility})
endmacro()

function(_jafg_add_dependency_impl
    module_name
    dep_path
    visibility
    )

    #
    # This dependency resolution works but only if the modules do not share the same name.
    # We have to resolve this later on... by linking against the .so / .a libs directly.
    #

    get_property(module_path GLOBAL PROPERTY "_jafg_added_modules_map_name_${module_name}")
    if(NOT DEFINED module_path)
        message(FATAL_ERROR "[${module_name}]: Could not resolve jafg module path.")
    endif()
    if(module_path STREQUAL "")
        message(FATAL_ERROR "[${module_name}]: Could not resolve jafg module path.")
    endif()

    message(STATUS "[${module_path}]: Adding dependency [${dep_path}].")

    get_property(dep_kind GLOBAL PROPERTY "_jafg_added_modules_map_path_${dep_path}")
    if(NOT DEFINED dep_kind)
        message(FATAL_ERROR "[${module_path}]: Could not resolve module [${dep_path}] as a jafg dependency.")
    endif()

    get_filename_component(dep_name "${dep_path}" NAME)
    if(${module_name} STREQUAL "")
        message(FATAL_ERROR "[${module_path}]: Could not resolve jafg module name form dep path [${dep_path}].")
    endif()
    get_property(is_found GLOBAL PROPERTY "_jafg_added_modules_map_name_${dep_name}")
    if(NOT DEFINED is_found)
        message(FATAL_ERROR "[${module_path}]: Could not resolve module [${dep_path}] as a jafg dependency.")
    endif()

    if(visibility STREQUAL "public")
        target_link_libraries(${module_name} PUBLIC
            ${dep_name}
            )
    elseif(visibility STREQUAL "private")
        target_link_libraries(${module_name} PRIVATE
            ${dep_name}
            )
    else()
        message(FATAL_ERROR "[${module_path}]: Invalid visibility [${visibility}] for dependency [${dep_path}].")
    endif()
endfunction()
