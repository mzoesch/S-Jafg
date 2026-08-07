# Copyright mzoesch. All rights reserved.

cmake_minimum_required(VERSION      4.2)

# C++26 is required; core Jafg systems will not compile with any other standard.
set(CMAKE_CXX_STANDARD              26)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

set(CMAKE_EXPERIMENTAL_CXX_MODULE_DYNDEP    0)
set(CMAKE_EXPERIMENTAL_CXX_MODULES          OFF)

# The root directory of the Jafg engine. Always valid. Use this to reference files in the Jafg engine.
set(JAFG_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})
message(STATUS "JAFG_ENGINE_ROOT: ${JAFG_ENGINE_ROOT}")
if(NOT EXISTS "${JAFG_ENGINE_ROOT}/jafg.jafgworkspace")
    message(FATAL_ERROR "[${JAFG_ENGINE_ROOT}/jafg.jafgworkspace]: No such file.")
endif()

###############################################################################
# Jafg Target Configuration.
###############################################################################
# The following are all platforms that Jafg supports.
set(JAFG_PLATFORM_LINUX     "lnx")
set(JAFG_PLATFORM_WINDOWS   "win")
set(JAFG_PLATFORM_WASM      "wasm")
set(JAFG_ALLOWED_PLATFORMS          ${JAFG_PLATFORM_LINUX} ${JAFG_PLATFORM_WINDOWS} ${JAFG_PLATFORM_WASM})
# The following are all architectures that Jafg supports.
set(JAFG_ARCHITECTURE_x64   "x86_64")
set(JAFG_ARCHITECTURE_x86   "x86")
set(JAFG_ALLOWED_ARCHITECTURES      ${JAFG_ARCHITECTURE_x64} ${JAFG_ARCHITECTURE_x86})
# The following are all targets that Jafg supports.
set(JAFG_TARGET_EDITOR      "editor")
set(JAFG_TARGET_CLIENT      "cl")
set(JAFG_TARGET_DAEMON      "daemon")
set(JAFG_ALLOWED_TARGETS            ${JAFG_TARGET_EDITOR} ${JAFG_TARGET_CLIENT} ${JAFG_TARGET_DAEMON})
# The following are all configurations that Jafg supports.
set(JAFG_CONFIG_DEBUG       "dbg")
set(JAFG_CONFIG_DEVELOPMENT "dev")
set(JAFG_CONFIG_SHIPPING    "ship")
set(JAFG_ALLOWED_CONFIGS            ${JAFG_CONFIG_DEBUG} ${JAFG_CONFIG_DEVELOPMENT} ${JAFG_CONFIG_SHIPPING})

# Default the variables if not defined.
if(NOT DEFINED JAFG_TARGET_PLATFORM)
    if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Linux")
        set(JAFG_TARGET_PLATFORM JAFG_PLATFORM_LINUX)
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Emscripten")
        set(JAFG_TARGET_PLATFORM JAFG_PLATFORM_WASM)
    elseif(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
        set(JAFG_TARGET_PLATFORM JAFG_PLATFORM_WINDOWS)
    else()
        message(FATAL_ERROR "JAFG_TARGET_PLATFORM is not defined and could not be predicted for the host system [${CMAKE_HOST_SYSTEM_NAME}]. "
            "Please define it with -DJAFG_TARGET_PLATFORM=<platform> or use a given preset with --preset \"<preset>\"."
            )
    endif()
    message(STATUS "JAFG_TARGET_PLATFORM implicitly set to [${JAFG_TARGET_PLATFORM}] on host system [${CMAKE_HOST_SYSTEM_NAME}].")
endif()
if(NOT DEFINED JAFG_TARGET_ARCHITECTURE)
    if(NOT DEFINED CMAKE_SYSTEM_PROCESSOR)
        message(FATAL_ERROR "JAFG_TARGET_ARCHITECTURE is not defined and could not be predicted for the host system [${CMAKE_HOST_SYSTEM_NAME}]. "
            "Please define it with -DJAFG_TARGET_ARCHITECTURE=<architecture> or use a given preset with --preset \"<preset>\"."
            )
    endIf()
    if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64})
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64})
    elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86")
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x86})
    # Fallback to the usual architecture for the current target platform.
    elseif(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_LINUX})
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64})
    elseif(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_WINDOWS})
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x64})
    elseif(JAFG_TARGET_PLATFORM STREQUAL ${JAFG_PLATFORM_WASM})
        set(JAFG_TARGET_ARCHITECTURE ${JAFG_ARCHITECTURE_x86})
    endif()
    message(STATUS "JAFG_TARGET_ARCHITECTURE implicitly set to [${JAFG_ARCHITECTURE_x64}] on [${JAFG_TARGET_PLATFORM}@${CMAKE_SYSTEM_PROCESSOR}].")
endif()
if(NOT DEFINED JAFG_TARGET_TYPE)
    set(JAFG_TARGET_TYPE JAFG_TARGET_CLIENT)
    message(STATUS "JAFG_TARGET_TYPE implicitly set to [${JAFG_TARGET_CLIENT}].")
endif()
if(NOT DEFINED JAFG_TARGET_CONFIG)
    if(DEFINED CMAKE_BUILD_TYPE)
        if(CMAKE_BUILD_TYPE STREQUAL "Debug")
            set(JAFG_TARGET_CONFIG JAFG_CONFIG_DEBUG)
        elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
            set(JAFG_TARGET_CONFIG JAFG_CONFIG_DEVELOPMENT)
        elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
            set(JAFG_TARGET_CONFIG JAFG_CONFIG_SHIPPING)
        else()
            message(FATAL_ERROR "Unknown CMAKE_BUILD_TYPE [${CMAKE_BUILD_TYPE}].")
        endif()
    else()
        set(JAFG_TARGET_CONFIG JAFG_CONFIG_SHIPPING)
    endif()
    message(STATUS "JAFG_TARGET_CONFIG implicitly set to [${JAFG_CONFIG_SHIPPING}].")
endif()
message(STATUS "JAFG_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM}")
message(STATUS "JAFG_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE}")
message(STATUS "JAFG_TARGET_TYPE: ${JAFG_TARGET_TYPE}")
message(STATUS "JAFG_TARGET_CONFIG: ${JAFG_TARGET_CONFIG}")

# Validate input.
if(NOT JAFG_TARGET_PLATFORM IN_LIST JAFG_ALLOWED_PLATFORMS)
    message(FATAL_ERROR "No such JAFG_TARGET_PLATFORM: ${JAFG_TARGET_PLATFORM} in [${JAFG_ALLOWED_PLATFORMS}].")
endif()
if(NOT JAFG_TARGET_ARCHITECTURE IN_LIST JAFG_ALLOWED_ARCHITECTURES)
    message(FATAL_ERROR "No such JAFG_TARGET_ARCHITECTURE: ${JAFG_TARGET_ARCHITECTURE} in [${JAFG_ALLOWED_ARCHITECTURES}].")
endif()
if(NOT JAFG_TARGET_TYPE IN_LIST JAFG_ALLOWED_TARGETS)
    message(FATAL_ERROR "No such JAFG_TARGET_TYPE: ${JAFG_TARGET_TYPE} in [${JAFG_ALLOWED_TARGETS}].")
endif()
if(NOT JAFG_TARGET_CONFIG IN_LIST JAFG_ALLOWED_CONFIGS)
    message(FATAL_ERROR "No such JAFG_TARGET_CONFIG: ${JAFG_TARGET_CONFIG} in [${JAFG_ALLOWED_CONFIGS}].")
endif()

# Convenience compound variables.
set(JAFG_PLATFORM_COMPOUND "${JAFG_TARGET_PLATFORM}-${JAFG_TARGET_ARCHITECTURE}")
set(JAFG_TARGET_COMPOUND "${JAFG_TARGET_TYPE}-${JAFG_TARGET_CONFIG}")
set(JAFG_COMPOUND_CONFIG_PATH "${JAFG_PLATFORM_COMPOUND}/${JAFG_TARGET_COMPOUND}")
message(STATUS "JAFG_PLATFORM_COMPOUND: ${JAFG_PLATFORM_COMPOUND}")
message(STATUS "JAFG_TARGET_COMPOUND: ${JAFG_TARGET_COMPOUND}")
message(STATUS "JAFG_COMPOUND_CONFIG_PATH: ${JAFG_COMPOUND_CONFIG_PATH}")

# Engine version.
if(NOT EXISTS "${JAFG_ENGINE_ROOT}/Config/.ver")
    message(FATAL_ERROR "[${JAFG_ENGINE_ROOT}/Config/.ver]: No such file.")
endif()
file(READ "${JAFG_ENGINE_ROOT}/Config/.ver" _version_content)
string(STRIP "${_version_content}" _detail_jafg_version)
if(NOT _detail_jafg_version)
    message(FATAL_ERROR "[${JAFG_ENGINE_ROOT}/Config/.ver]: Failed to retrieve version number.")
endif()
message(STATUS "JAFG_VERSION: ${_detail_jafg_version}")

# Cpp version.
if(NOT EXISTS "${JAFG_ENGINE_ROOT}/Config/.__cplusplus")
    message(FATAL_ERROR "[${JAFG_ENGINE_ROOT}/Config/.__cplusplus]: No such file.")
endif()
file(READ "${JAFG_ENGINE_ROOT}/Config/.__cplusplus" _detail_jafg_min_cplusplus_content)
string(STRIP "${_detail_jafg_min_cplusplus_content}" _detail_jafg_min_cplusplus)
if(NOT _detail_jafg_min_cplusplus)
    message(FATAL_ERROR "[${JAFG_ENGINE_ROOT}/Config/.__cplusplus]: Failed to retrieve version number.")
endif()
message(STATUS "DETAIL_JAFG_CPLUSPLUS: ${_detail_jafg_min_cplusplus}")

# Launcher.
option(JAFG_USE_CCACHE "Use ccache to speed up compilation in development builds." On)
option(JAFG_FORCE_USE_CCACHE "Always use ccache even in shipping builds." Off)
message(STATUS "JAFG_USE_CCACHE: ${JAFG_USE_CCACHE}")
message(STATUS "JAFG_FORCE_USE_CCACHE: ${JAFG_FORCE_USE_CCACHE}")
if(JAFG_FORCE_USE_CCACHE OR (JAFG_USE_CCACHE AND NOT (JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)))
    find_program(CCACHE_PROGRAM ccache)
    if(CCACHE_PROGRAM)
        message(STATUS "ccache: ${CCACHE_PROGRAM}")
        if(NOT DEFINED CMAKE_C_COMPILER_LAUNCHER)
            message(STATUS "CMAKE_C_COMPILER_LAUNCHER: ${CCACHE_PROGRAM}")
            set(CMAKE_C_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        endif()
        if(NOT DEFINED CMAKE_CXX_COMPILER_LAUNCHER)
            message(STATUS "CMAKE_CXX_COMPILER_LAUNCHER: ${CCACHE_PROGRAM}")
            set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
        endif()
    else()
        message(FATAL_ERROR "[ccache]: Not found. Either disable JAFG_USE_CCACHE/JAFG_FORCE_USE_CCACHE or install ccache.")
    endif()
endif()

if(NOT CMAKE_BUILD_TYPE)
    if(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG)
        set(CACHE CMAKE_BUILD_TYPE Debug FORCE)
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT)
        set(CACHE CMAKE_BUILD_TYPE RelWithDebInfo FORCE)
    elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
        set(CACHE CMAKE_BUILD_TYPE Release FORCE)
    else()
        message(FATAL_ERROR "Could not infer CMAKE_BUILD_TYPE.")
    endif()
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

###############################################################################
# Project setup.
###############################################################################
project(
    Jafg
    VERSION         "${_detail_jafg_version}"
    LANGUAGES       C CXX
    DESCRIPTION     ""
    HOMEPAGE_URL    "https://github.com/mzoesch/S-Jafg"
    )

# Required CMake extensions.
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)
include(FetchContent)
include(ExternalProject)

include(CMake/Utilities.cmake)

message(STATUS "CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "CMAKE_CURRENT_BINARY_DIR: ${CMAKE_CURRENT_BINARY_DIR}")
message(STATUS "CMAKE_C_STANDARD: ${CMAKE_C_STANDARD}")
message(STATUS "CMAKE_C_COMPILER_ID: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_STANDARD: ${CMAKE_CXX_STANDARD}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")

if(CMAKE_CONFIGURATION_TYPES)
    message(FATAL_ERROR "Multi-configuration generators are not allowed. Please use a single-configuration generator with -G <generator>.")
endif()

message(STATUS "Checking for in-source build ...")
get_filename_component(_detail_jafg_src_dir "${CMAKE_SOURCE_DIR}" REALPATH)
get_filename_component(_detail_jafg_bin_dir "${CMAKE_BINARY_DIR}" REALPATH)
if("${_detail_jafg_src_dir}" STREQUAL "${_detail_jafg_bin_dir}")
    message(FATAL_ERROR "In-source build detected. Aborting CMake configuration.")
endif()

# Ensure the compiler. Jafg only supports a handful of compilers.
message(STATUS "Checking C and CXX compiler for platform [${JAFG_TARGET_PLATFORM}].")
if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
        message(FATAL_ERROR "CXX compiler [clang] or [gcc] is required for this platform [${JAFG_TARGET_PLATFORM}]. Current compiler [${CMAKE_CXX_COMPILER_ID}] with c[${CMAKE_C_COMPILER}] and cxx[${CMAKE_CXX_COMPILER}].")
    endif()
elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
    if(NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang|MSVC")
        message(FATAL_ERROR "CXX compiler [clang] or [msvc] is required for this platform [${JAFG_TARGET_PLATFORM}]. Current compiler [${CMAKE_CXX_COMPILER_ID}] with c[${CMAKE_C_COMPILER}] and cxx[${CMAKE_CXX_COMPILER}].")
    endif()
elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WASM)
    string(REGEX MATCH ".*em\\+\\+.*" REGREX_MATCHED ${CMAKE_CXX_COMPILER})
    if(NOT REGEX_MATCHED)
        message(FATAL_ERROR "CXX compiler [em++] is required for this platform [${JAFG_TARGET_PLATFORM}]. Current compiler [${CMAKE_CXX_COMPILER_ID}] with c[${CMAKE_C_COMPILER}] and cxx[${CMAKE_CXX_COMPILER}].")
    endif()
else()
    message(FATAL_ERROR "Missing implementation here for JAFG_TARGET_PLATFORM [${JAFG_TARGET_PLATFORM}].")
endif()

if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} "${JAFG_ENGINE_ROOT}/msys64/ucrt64")
endif()
set(FETCHCONTENT_UPDATES_DISCONNECTED ON)
# The performance cost is neglectable for the insane amount of benefits it brings. So just enable globally.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
UpdateSubmodules()
if(JAFG_DO_SANITIZED_BUILD)
    SanitizeThisBuildGlobally()
endif()
if(JAFG_DO_HARDEN_BUILD)
    HardenThisBuildGlobally()
endif()

include(CMake/Motor.cmake)
if(NOT EXISTS "${JAFG_MOTOR_EXECUTABLE}")
    message(STATUS "[${JAFG_MOTOR_EXECUTABLE}]: No such file. Recompiling.")
    RecompileMotor()
else()
    message(STATUS "[${JAFG_MOTOR_EXECUTABLE}]: Valid. Skipping compilation.")
endif()
if(NOT EXISTS "${JAFG_MOTOR_EXECUTABLE}")
    message(FATAL_ERROR "Motor executable [${JAFG_MOTOR_EXECUTABLE}] could not be found or compilation process failed silently.")
endif()

include(CMake/ModuleFlags.cmake)

###############################################################################
# Jafg Foreign Public Logic.
###############################################################################

#
# These are the search paths for Jafg to check for modules to be loaded.
# You may add your own paths to this list.
#
set(JAFG_SEARCH_PATHS ${JAFG_SEARCH_PATHS} "Engine" "Plugins" "Runtime")

#
# Exclude paths are used to ignore certain directories in the search paths for Jafg modules.
# You may add your own to this list.
#
set(JAFG_EXCLUDE_PATHS ${JAFG_EXCLUDE_PATHS} "Plugins/Vendor")

# A static module that is linked into the program at compile time.
set(JAFG_MODULE_TYPE_STATIC     "static")
# This module is a "normal" shared library that is loaded before the program starts. It is required to be available
# at runtime. This module has to be linked against at compile time.
set(JAFG_MODULE_TYPE_SHARED     "shared")
# This module must define an entry point to link against. It is bad design to link against this module type.
set(JAFG_MODULE_TYPE_LAUNCH     "launch")
# This module is a plugin that is loaded dynamically at runtime based of user preferences or transitive
# plugin dependencies. It is not required by the language to be available at runtime.
set(JAFG_MODULE_TYPE_PLUGIN     "plugin")
# Convenience functions.
function(jafg_is_type_shared module_type)
    return(${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED)
endfunction()
function(jafg_is_type_static module_type)
    return(${module_type} STREQUAL JAFG_MODULE_TYPE_STATIC)
endfunction()
function(jafg_is_type_launch module_type)
    return(${module_type} STREQUAL JAFG_MODULE_TYPE_LAUNCH)
endfunction()
function(jafg_is_type_plugin module_type)
    return(${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
endfunction()

#
# Use this macro to add your own modules to the Jafg solution.
# After that use the normal CMake commands to customize the just added module.
# The module name can be accessed via "_scoped_module_name". It is the name of the directory the CMakeLists.txt is in,
# e.g. this means that:
#    Runtime                    => Runtime
#    Engine                     => Engine
#    Plugins/mzoesch/MyPlugin   => MyPlugin
#
# Your CMakeLists.txt file may be loaded multiple times by the Jafg build tool, therefore it is necessary to guard
# it, e.g.:
#     include_guard(_guard)                             # If this CMakeLists.txt was located in the "Plugins/MyPlugin"
#     if(_guard)                                        # directory, then this just added a new plugin for Jafg with
#         jafg_add_module(JAFG_MODULE_TYPE_PLUGIN)      # the name "MyPlugin" and identifier "Plugins/MyPlugin".
#         < Custom CMake commands here if necessary >   # This module can now be dynamically loaded at runtime.
#     endif()
#
# You may set the following variables before calling this macro:
#   this_plugin_identifier:             Set a custom identifier for your module. Default is [_scoped_module_name].
#   this_plugin_lifetime_identifier:    Set a custom lifetime identifier for your module. Default is [this_plugin_identifier].
#   this_plugin_friendly_name:          Set a custom friendly name for your module. Default is [_scoped_module_name].
#   this_plugin_description:            Set a custom description for your module. Default is [""].
#   this_plugin_author:                 Set a custom author for your module. Default is ["Anonymous"].
#   this_plugin_supports_dyn_unload:    Set to false if your module does not support dynamic unloading at runtime. Default is [true].
#
# @see Examples can be found at "Engine/CMakeLists.txt" or at "Plugins/Jgc/CMakeLists.txt".
#
macro(jafg_add_module
    # The type of the module. See the #JAFG_MODULE_TYPE_* constants above for valid values.
    module_type
    )
    _jafg_add_module(${module_type})
endmacro()

#
# Add another Jafg module as a dependency to the current module.
# @see Examples can be found at "Plugins/Jgc/CMakeLists.txt".
#
macro(jafg_add_dependency
    # The relative path of the module to add as a dependency to the engine. E.g.: "Engine".
    dep_path
    #
    # The visibility of the dependency. This is either "public" or "private".
    # Public dependencies are linked and added transitively to the current module.
    # Meaning if another module depends on this module, the dependency will be added to the other module as well.
    # Private dependencies are only linked to the current module.
    #
    visibility
    )
    _jafg_add_dependency(${dep_path} ${visibility})
endmacro()

#
# A header guard macro that only allows a file to be included once in the current CMake context.
# Use it like this:
# include_guard(_guard)
# if(_guard)
#    /* ... Your code here ... */
# endif()
#
macro(include_guard Out)
    string(REPLACE "/" "_" guard_path "${CMAKE_CURRENT_LIST_FILE}")
    string(REPLACE "\\" "_" guard_path "${guard_path}")
    string(REPLACE " " "_" guard_path "${guard_path}")
    string(TOUPPER "${guard_path}" guard_name)
    set(guard "_jafg_include_guard_${guard_name}")

    get_property(retrieved_guard GLOBAL PROPERTY "${guard}")
    if(NOT DEFINED retrieved_guard)
        set_property(GLOBAL PROPERTY "${guard}" "TRUE")
        set(${Out} "TRUE")
    else()
        set(${Out} "FALSE")
    endif()
endmacro()

# Whether this type behaves like a shared module. So whether we have to be cautious about exporting symbols or not.
function(jafg_is_shared module_type)
    if(${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED)
        return(TRUE)
    elseif(${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
        return(TRUE)
    else()
        return(FALSE)
    endif()
endfunction()

include(CMake/ModuleUtilities.cmake)

#
# This fetches all modules that are either part of Jafg or foreign.
# To write your own module and load it at runtime into the Jafg engine, create a new directory in the
# "Plugins" directory or any subdirectory of it. In said directory, create a CMakeLists.txt file that
# contains the module's CMake configuration.
#
# @see Example can be found at "Plugins/Jgc/CMakeLists.txt".
#
foreach(search_path ${JAFG_SEARCH_PATHS})
    message(VERBOSE "Searching for modules in [${search_path}] ...")

    file(GLOB children_flat RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${search_path}/CMakeLists.txt")
    foreach(child ${children_flat})
        get_filename_component(child_dir "${child}" DIRECTORY)

        set(exclude_found FALSE)
        foreach(exclude_path ${JAFG_EXCLUDE_PATHS})
            if(child_dir MATCHES "^${exclude_path}")
                set(exclude_found TRUE)
                break()
            endif()
        endforeach()

        if(exclude_found)
            message(STATUS "Skipping module at [${child_dir}] due to exclusion.")
            continue()
        endif()

        message(STATUS "Found module at [${child_dir}].")
        add_subdirectory("${child_dir}")
    endforeach()

    file(GLOB_RECURSE children_recursive RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/${search_path}/*/CMakeLists.txt")
    foreach(child ${children_recursive})
        get_filename_component(child_dir "${child}" DIRECTORY)

        set(exclude_found FALSE)
        foreach(exclude_path ${JAFG_EXCLUDE_PATHS})
            if(child_dir MATCHES "^${exclude_path}")
                set(exclude_found TRUE)
                break()
            endif()
        endforeach()

        if(exclude_found)
            message(STATUS "Skipping module at [${child_dir}] due to exclusion.")
            continue()
        endif()

        message(STATUS "Found module at [${child_dir}].")
        add_subdirectory("${child_dir}")
    endforeach()
endforeach()
