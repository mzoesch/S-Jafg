# Copyright mzoesch. All rights reserved.

###############################################################################
# This file contains utility functions and macros that may be useful
# for Jafg modules.
###############################################################################

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

#
# Whether this type behaves like a shared module. So whether we have to be cautious about exporting symbols or not.
#
function(jafg_is_shared module_type)
    if(${module_type} STREQUAL JAFG_MODULE_TYPE_SHARED)
        return(TRUE)
    elseif(${module_type} STREQUAL JAFG_MODULE_TYPE_PLUGIN)
        return(TRUE)
    else()
        return(FALSE)
    endif()
endfunction()

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
