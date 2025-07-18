# Copyright mzoesch. All rights reserved.

function(retrieve_version_number p_version)
    if(NOT EXISTS "${JAFG_ENGINE_ROOT}/Config/.ver")
        message(FATAL_ERROR "Version file not found at ${JAFG_ENGINE_ROOT}/Config/.ver")
    endif()

    file(READ "${JAFG_ENGINE_ROOT}/Config/.ver" _version_content)
    string(STRIP "${_version_content}" _version)

    if(NOT _version)
        message(FATAL_ERROR "Version number could not be retrieved from Config/.ver")
    endif()

    set(${p_version} "${_version}" PARENT_SCOPE)
    message(STATUS "Retrieved version number from config file to variable [${p_version}].")
endfunction()

retrieve_version_number(_private_jafg_version)
message(STATUS "Jafg version: [${_private_jafg_version}].")
