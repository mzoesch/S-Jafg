# Copyright mzoesch. All rights reserved.

function(get_min_cplusplus p_min_cplusplus)
    if(NOT EXISTS "${JAFG_ENGINE_ROOT}/Config/.__cplusplus")
        message(FATAL_ERROR "Version file not found at ${JAFG_ENGINE_ROOT}/Config/.__cplusplus")
    endif()

    file(READ "${JAFG_ENGINE_ROOT}/Config/.__cplusplus" _min_cplusplus_content)
    string(STRIP "${_min_cplusplus_content}" _min_cplusplus)

    if(NOT _min_cplusplus)
        message(FATAL_ERROR "Version number could not be retrieved from Config/.__cplusplus")
    endif()

    set(${p_min_cplusplus} "${_min_cplusplus}" PARENT_SCOPE)
    message(STATUS "Retrieved version number from config file to variable [${p_min_cplusplus}].")
endfunction()

get_min_cplusplus(_private_jafg_min_cplusplus)
message(STATUS "Jafg min __cplusplus: [${_private_jafg_min_cplusplus}].")
