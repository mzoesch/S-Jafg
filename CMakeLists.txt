# Copyright mzoesch. All rights reserved.

# Update to 4.0 when CLion supports it.
cmake_minimum_required(VERSION      3.30)

# C++23 is required; core Jafg systems will not compile with any other standard.
set(CMAKE_CXX_STANDARD              23)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

include(Programs/WhatTheFuckIsThat.cmake)

#
# The root directory of the Jafg engine. Always valid. Use this to reference files in the Jafg engine.
#
set(JAFG_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

# The target platform to compile for [Linux, Wasm, Windows].
if(NOT DEFINED JAFG_TARGET_PLATFORM)
    message(FATAL_ERROR "JAFG_TARGET_PLATFORM is not defined. Please define it with -DJAFG_TARGET_PLATFORM=<platform>.")
endif()
# The target to compile for [Client, Daemon, TestUnit].
if(NOT DEFINED JAFG_TARGET_TYPE)
    message(FATAL_ERROR "JAFG_TARGET_TYPE is not defined. Please define it with -DJAFG_TARGET_TYPE=<target>.")
endif()
# The configuration to compile for [Debug, Development, Shipping].
if(NOT DEFINED JAFG_TARGET_CONFIG)
    message(FATAL_ERROR "JAFG_TARGET_CONFIG is not defined. Please define it with -JAFG_TARGET_CONFIG=<configuration>.")
endif()
include(Programs/ValidateTargetInput.cmake)

include(Programs/ValidateCompiler.cmake)

project(
    Jafg
    VERSION         0.0.1
    LANGUAGES       C CXX
    DESCRIPTION     ""
    HOMEPAGE_URL    "https://github.com/mzoesch/S-Jafg"
    )

include(Programs/EarlySetup.cmake)

#
# These are the search paths for Jafg to check for modules to be loaded.
# You may add your own paths to this list.
#
set(JAFG_SEARCH_PATHS
    "Engine"
    "Lal"
    "Plugins"
    "Runtime"
    )

#
# Exclude paths are used to ignore certain directories in the search paths for Jafg modules.
# You may add your onw to this list.
#
set(JAFG_EXCLUDE_PATHS
    "Engine/Vendor"
    "Plugins/Vendor"
    )

# A static module that is linked into the program at compile time.
set(JAFG_MODULE_TYPE_STATIC     "static")
# This module is a "normal" shared library that is loaded before the program starts. It is required to be available
# at runtime. This module has to be linked against at compile time.
set(JAFG_MODULE_TYPE_SHARED     "shared")
# This module must define an entry point to link against it. It is bad design to link against this module type.
set(JAFG_MODULE_TYPE_LAUNCH     "launch")
# This module is a plugin that is loaded at runtime. It is not required by the language to be available at runtime.
set(JAFG_MODULE_TYPE_PLUGIN     "plugin")

#
# Use this macro to add your module to the Jafg solution.
# After that use the normal CMake commands to customize the just added module.
# @see Examples can be found at "Engine/Engine/CMakeLists.txt" or at "Plugins/JafgPlugin/CMakeLists.txt".
#
macro(jafg_add_module
    # The type of the module. See the #JAFG_MODULE_TYPE_* constants above for valid values.
    module_type
    )
    _jafg_add_module(${module_type})
endmacro()

#
# Add another Jafg module as a dependency to the current module.
# @see Examples can be found at "Engine/EngineForward/CMakeLists.txt" or at "Plugins/JafgPlugin/CMakeLists.txt".
#
macro(jafg_add_dependency
    # The relative path of the module to add as a dependency to the engine.
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

include(Programs/LateSetup.cmake)

#
# This fetches all modules that are either part of Jafg or foreign.
# To write your own module and load it at runtime into the Jafg engine, create a new directory in the
# "Plugins" directory or any subdirectory of it. In said directory, create a CMakeLists.txt file that
# contains the module's CMake configuration.
# @see Example can be found at "Plugins/JafgPlugin/CMakeLists.txt".
#
include(Programs/ModuleFetcher.cmake)
