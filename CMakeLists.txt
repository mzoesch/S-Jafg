# Copyright mzoesch. All rights reserved.

# Update to 4.0 when CLion supports it.
cmake_minimum_required(VERSION      3.30)

# C++23 is required; core Jafg systems will not compile with any other standard.
set(CMAKE_CXX_STANDARD              23)
set(CMAKE_CXX_STANDARD_REQUIRED     ON)
set(CMAKE_CXX_EXTENSIONS            OFF)

include(CMake/WhatTheFuckIsThat.cmake)


#
# The root directory of the Jafg engine. Always valid. Use this to reference files in the Jafg engine.
#
set(JAFG_ENGINE_ROOT ${CMAKE_CURRENT_SOURCE_DIR})


###############################################################################
# Jafg Target Configuration.
###############################################################################
# Try to configure the targets of Jafg automatically based of the host system and / or other variables.
include(CMake/ConfigurationPrediction.cmake)
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
    message(FATAL_ERROR "JAFG_TARGET_CONFIG is not defined. Please define it with -DJAFG_TARGET_CONFIG=<configuration>.")
endif()
include(CMake/ValidateTargetInput.cmake)
include(CMake/ValidateCompiler.cmake)
include(CMake/RetrieveVersionNumber.cmake)
include(CMake/RetrieveMinCPlusPlus.cmake)
if(NOT CMAKE_BUILD_TYPE)
    if(JAFG_TARGET_CONFIG STREQUAL "Debug")
        set(CMAKE_BUILD_TYPE Debug)
    elseif(JAFG_TARGET_CONFIG STREQUAL "Development")
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
    elseif(JAFG_TARGET_CONFIG STREQUAL "Shipping")
        set(CMAKE_BUILD_TYPE Release)
    else()
        message(FATAL_ERROR "Could not infer CMAKE_BUILD_TYPE.")
    endif()
endif()

project(
    Jafg
    VERSION         "${_detail_jafg_version}"
    LANGUAGES       C CXX
    DESCRIPTION     ""
    HOMEPAGE_URL    "https://github.com/mzoesch/S-Jafg"
    )


include(CMake/EarlySetup.cmake)


###############################################################################
# Jafg Module Public Variables.
###############################################################################

#
# These are the search paths for Jafg to check for modules to be loaded.
# You may add your own paths to this list.
#
if(NOT DEFINED JAFG_SEARCH_PATHS)
    set(JAFG_SEARCH_PATHS
        "Engine"
        "Plugins"
        "Runtime"
        )
endif()

#
# Exclude paths are used to ignore certain directories in the search paths for Jafg modules.
# You may add your own to this list.
#
if(NOT DEFINED JAFG_EXCLUDE_PATHS)
    set(JAFG_EXCLUDE_PATHS
        "Plugins/Vendor"
        )
endif()

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


###############################################################################
# Jafg Foreign Public Logic.
###############################################################################

#
# Use this macro to add your own modules to the Jafg solution.
# After that use the normal CMake commands to customize the just added modules.
# The module name can be accessed via "_scoped_module_name". It is the name of the directory the CMakeLists.txt is in,
# e.g. this means that:
#    Runtime                    => Runtime
#    Engine                     => Engine
#    Plugins/mzoesch/MyPlugin   => MyPlugin
#
# You CMakeLists.txt file may be loaded multiple times by the Jafg build tool, therefore it is necessary to guard
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

include(CMake/LateSetup.cmake)

#
# This fetches all modules that are either part of Jafg or foreign.
# To write your own module and load it at runtime into the Jafg engine, create a new directory in the
# "Plugins" directory or any subdirectory of it. In said directory, create a CMakeLists.txt file that
# contains the module's CMake configuration.
#
# @see Example can be found at "Plugins/Jgc/CMakeLists.txt".
#
include(CMake/ModuleFetcher.cmake)
