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
        message(FATAL_ERROR "Expected file at [${p_file_path}].")
    endif()

    set(${p_content} "${_content}" PARENT_SCOPE)
endfunction()
