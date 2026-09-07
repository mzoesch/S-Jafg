# Copyright mzoesch. All rights reserved.

macro(_jafg_add_module
    module_type
    )
    get_filename_component(module_name "${CMAKE_CURRENT_LIST_DIR}" NAME)
    _jafg_add_module_impl(${module_name} ${module_type} "${CMAKE_CURRENT_LIST_DIR}")
    set(_scoped_module_name "${module_name}")
endmacro()

macro(_jafg_meta_plugin_flag
    flag
    default_value
    )
    if(DEFINED ${flag})
        set(_${flag} ${${flag}})
        unset(${flag} PARENT_SCOPE)
    else()
        set(_${flag} ${default_value})
    endif()
    message(STATUS "[Meta Flag] ${flag} = ${_${flag}}")
endmacro()

macro(_jafg_add_option_if_true option)
    if(${option})
        target_compile_definitions(${module_name} PRIVATE ${option}=1)
    endif()
    message(STATUS "${option}: ${${option}}")
endmacro()

macro(add_preference_if_specified preference)
    if(NOT ${preference} STREQUAL "Unspecified")
        target_compile_definitions(${module_name} PRIVATE
            ${preference}=${${preference}}
            )
    endif()
    message(STATUS "${preference}: ${${preference}}")
endmacro()
macro(add_preference_if_specified_or preference default)
    if(NOT ${preference} STREQUAL "Unspecified")
        target_compile_definitions(${module_name} PRIVATE
            ${preference}=${${preference}}
            )
        message(STATUS "${preference}: ${${preference}}")
    else()
        target_compile_definitions(${module_name} PRIVATE
            ${preference}=${default}
            )
        message(STATUS "${preference}: ${default}")
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

    file(GLOB_RECURSE src_files
        "${module_dir}/include/*.afx"
        "${module_dir}/include/*.pch"
        "${module_dir}/include/*.h"
        "${module_dir}/include/*.hpp"
        "${module_dir}/src/*.afx"
        "${module_dir}/src/*.pch"
        "${module_dir}/src/*.h"
        "${module_dir}/src/*.hpp"
        "${module_dir}/src/*.c"
        "${module_dir}/src/*.cpp"
        "${CMAKE_CURRENT_BINARY_DIR}/gh/*.h"
        "${CMAKE_CURRENT_BINARY_DIR}/gt/*.cpp"
        )

    file(GLOB_RECURSE src_files_c
        "${module_dir}/src/*.c"
        )

    if(${module_type} STREQUAL JAFG_MODULE_TYPE_LAUNCH)
        if((CMAKE_CXX_COMPILER_ID STREQUAL "MSVC") AND (JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING))
            add_executable(${module_name} WIN32 ${src_files})
        else()
            add_executable(${module_name} ${src_files})
        endif()
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
            ${module_name_upper}_API=JAFG_PLATFORM_CALLSPEC_OUT
            )
        target_compile_definitions(${module_name} INTERFACE
            ${module_name_upper}_API=JAFG_PLATFORM_CALLSPEC_IN
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
        "${module_dir}/include"
        "${CMAKE_CURRENT_BINARY_DIR}/gh"
        )
    target_include_directories(${module_name} PRIVATE
        "${CMAKE_CURRENT_BINARY_DIR}"
        )

    set(test_file "${CMAKE_CURRENT_BINARY_DIR}/gt/_TestModule.generated.cpp")
    if(EXISTS "${test_file}")
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            set_source_files_properties(${test_file} PROPERTIES COMPILE_FLAGS "-I${module_dir}/tests")
        elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
            set_source_files_properties(${test_file} PROPERTIES COMPILE_FLAGS "/I${module_dir}/tests")
        else()
            message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
        endif()
    endif()

    set(pch_file "${module_dir}/src/module.pch")
    if(EXISTS "${pch_file}")
        target_precompile_headers(${module_name} PRIVATE
            "${pch_file}"
            )
    endif()

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_PLATFORM_LINUX=1
            )
    elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_PLATFORM_WINDOWS=1
            )
    elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WASM)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_PLATFORM_WASM=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_PLATFORM [${JAFG_TARGET_PLATFORM}].")
    endif()

    target_compile_definitions(${module_name} PRIVATE
        DETAIL_JAFG_CPLUSPLUS=${_detail_jafg_min_cplusplus}
        )

    if(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_EDITOR)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_WITH_LOCAL_LAYER=1
            JAFG_WITH_EDITOR=1
            )
    elseif(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_CLIENT)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_WITH_LOCAL_LAYER=1
            )
    elseif(JAFG_TARGET_TYPE STREQUAL JAFG_TARGET_DAEMON)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_AS_DAEMON=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_TYPE [${JAFG_TARGET_TYPE}].")
    endif()

    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_IN_DEBUG=1
            )
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_IN_DEVELOPMENT=1
            )
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        target_compile_definitions(${module_name} PRIVATE
            JAFG_IN_SHIPPING=1
            )
    else()
        message(FATAL_ERROR "Missing implementation for JAFG_TARGET_CONFIG [${JAFG_TARGET_CONFIG}].")
    endif()

    ###############################################################################
    # Compiler flags
    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        target_compile_options(${module_name} PRIVATE
            -fno-rtti                   # No RTTI.
            # -freflection                # Reflection yay!
            # -fcontracts                 # Contracts.
            # This is the default for clang, so just ignore it for now.
            # -stdlib=libstdc++         # Use libstdc++ instead of LLVM's libc++.
            -fno-exceptions             # No exceptions.
            -fno-common                 # Forces global variables to be defined in exactly one object file.
            -fvisibility=hidden         # Hides all symbols by default.
            -fvisibility-inlines-hidden # Fuck those inlines.
            -Wall -Wextra -Wpedantic    # Enable many warnings.
            -Wno-missing-include-dirs   # So unnecessary...
            -Werror                     # Warnings as errors
            )
        if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
            target_compile_options(${module_name} PRIVATE
                -Weverything
                -fsafe-buffer-usage-suggestions
                )
        endif()
#        target_link_options(${module_name} PRIVATE
#            -rdynamic
#            )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${module_name} PRIVATE
            /nologo                     # Annoying as shit.
            /permissive-                # Biggest mistake by Microsoft.
            /Zc:__cplusplus             # Why the fuck microsoft?
            /GR-                        # No RTTI.
            /MP                         # Multiple processors.
            /Zc:preprocessor            # No legacy pp.
            )
    else()
        message(FATAL_ERROR "Missing implementation for CMAKE_CXX_COMPILER_ID [${CMAKE_CXX_COMPILER_ID}].")
    endif()

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
        find_library(DBGHELP_LIBRARY dbghelp)
        if(DBGHELP_LIBRARY)
            target_link_libraries(${module_name} PRIVATE ${DBGHELP_LIBRARY})
        else()
            message(FATAL_ERROR "No such library: dbghelp.")
        endif()
    endif()

    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(${module_name} PRIVATE
                -g              # Debug symbols.
                -O0             # Prevent optimizations.
                -fno-omit-frame-pointer # Readability
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
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            target_compile_options(${module_name} PRIVATE
                -g              # Debug symbols.
                -O2             # Tsundere optimizations.
                -fno-omit-frame-pointer # Readability
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
        if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
            if(JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING)
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
            if(JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING)
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

    # Cmake options.
    _jafg_add_option_if_true(JAFG_DO_DEBUG_SYMBOLS_IN_SHIPPING)
    _jafg_add_option_if_true(JAFG_DO_SANITIZED_BUILD)
    _jafg_add_option_if_true(JAFG_DO_HARDEN_BUILD)

    # Misc flags.
    add_preference_if_specified(JAFG_DO_COMPILER_DIAGNOSTIC_SETUP)
    add_preference_if_specified(JAFG_DO_ENABLE_SHIPPING_WARNINGS)

    # Log flags.
    add_preference_if_specified(JAFG_LOG_DEFAULT_VERBOSITY)
    add_preference_if_specified(JAFG_LOG_ENABLE_TRACE)
    add_preference_if_specified(JAFG_LOG_ENABLE_VERBOSE)
    add_preference_if_specified(JAFG_LOG_ENABLE_INFO)
    add_preference_if_specified(JAFG_LOG_ENABLE_WARNING)
    add_preference_if_specified(JAFG_LOG_ENABLE_ERROR)
    add_preference_if_specified(JAFG_LOG_DO_SCOPED_TIME_TASK_MEASURER)
    add_preference_if_specified(JAFG_FORCE_LOG_FLUSH_INTERVAL)
    add_preference_if_specified(JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES)

    add_preference_if_specified(JAFG_WITH_STATS)
    add_preference_if_specified(JAFG_STATS_ON_DEMAND)
    add_preference_if_specified(JAFG_STATS_NO_EXIT)
    add_preference_if_specified(JAFG_MAX_FRAMES_IN_FLIGHT)
    # ~Compiler flags
    ###############################################################################

    add_custom_target(zzz_${module_name}_PRE_BUILD
        COMMAND ${JAFG_MOTOR_EXECUTABLE}
            --BinaryDir ${CMAKE_CURRENT_BINARY_DIR}
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
        set(_target_root_plugin_jafg "${CMAKE_CURRENT_BINARY_DIR}/manifest.jafg")
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

            _jafg_meta_plugin_flag(this_plugin_identifier "${module_name}")
            _jafg_meta_plugin_flag(this_plugin_lifetime_identifier ${_this_plugin_identifier})
            _jafg_meta_plugin_flag(this_plugin_friendly_name "${module_name}")
            _jafg_meta_plugin_flag(this_plugin_description "")
            _jafg_meta_plugin_flag(this_plugin_author "Anonymous")
            _jafg_meta_plugin_flag(this_plugin_supports_dyn_unload "true")

            set(_target_root_plugin_jafg_content
"{
    \"Version\": \"${_target_ver}\",
    \"Identifier\": \"${_this_plugin_identifier}\",
    \"LifetimeIdentifier\": \"${_this_plugin_lifetime_identifier}\",
    \"NativeIdentifier\": \"${module_name}\",
    \"FriendlyName\": \"${_this_plugin_friendly_name}\",
    \"Description\": \"${_this_plugin_description}\",
    \"Author\": \"${_this_plugin_author}\",
    \"bDynUnloadable\": ${_this_plugin_supports_dyn_unload},
    \"Bin\": \"${prefix}${module_name}${suffix}\"
}
")
#    \"Bin\": \"bin/${JAFG_COMPOUND_CONFIG_PATH}/${module_rel_dir}/${prefix}${module_name}${suffix}\"
            file(WRITE "${_target_root_plugin_jafg}" "${_target_root_plugin_jafg_content}")
            message(STATUS "${_target_root_plugin_jafg}: Created manifest file for plugin [${module_rel_dir}].")
        else()
            message(STATUS "${_target_root_plugin_jafg}: Already exists. Skipping.")
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
