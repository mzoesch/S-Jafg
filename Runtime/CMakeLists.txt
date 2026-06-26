# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_LAUNCH)
    jafg_add_dependency("Engine" "private")

    if((JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS) AND (NOT(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")))
        add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${JAFG_ENGINE_ROOT}/msys64/ucrt64/bin/libgcc_s_seh-1.dll"
            "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            )
        add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${JAFG_ENGINE_ROOT}/msys64/ucrt64/bin/libstdc++-6.dll"
            "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            )
        add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${JAFG_ENGINE_ROOT}/msys64/ucrt64/bin/libwinpthread-1.dll"
            "${JAFG_ENGINE_ROOT}/bin/${JAFG_COMPOUND_CONFIG_PATH}"
            )
    endif()
endif()
