# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_LAUNCH)
    jafg_add_dependency("Engine" "private")

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
        set_target_properties(${_scoped_module_name} PROPERTIES
            BUILD_RPATH "\$ORIGIN:\$ORIGIN/../Engine"
            )
    endif()

    if((JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS) AND (NOT(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")))
        # The directory that provides the MinGW runtime dlls: a local MSYS2 installation for native
        # builds, the sysroot of the dev container when cross compiling.
        if(CMAKE_CROSSCOMPILING)
            set(_mingw_bin_dir "${CMAKE_SYSROOT}/bin")
        else()
            set(_mingw_bin_dir "${JAFG_ENGINE_ROOT}/msys64/ucrt64/bin")
        endif()

        # Runtime dependencies
        foreach(_dll libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll)
            add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_mingw_bin_dir}/${_dll}"
                "${CMAKE_CURRENT_BINARY_DIR}"
                )
        endforeach()

        # Engine
        add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            $<TARGET_FILE:Engine>
            "${CMAKE_CURRENT_BINARY_DIR}"
            )

        # Engine dependencies. Only present when FreeType picked these libraries up from the system directly.
        foreach(_dll libbrotlidec.dll libbz2-1.dll zlib1.dll)
            if(CMAKE_CROSSCOMPILING AND (NOT EXISTS "${_mingw_bin_dir}/${_dll}"))
                continue()
            endif()
            add_custom_command(TARGET ${_scoped_module_name} POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${_mingw_bin_dir}/${_dll}"
                "${CMAKE_CURRENT_BINARY_DIR}"
                )
        endforeach()
    endif()
endif()
