# Copyright mzoesch. All rights reserved.

include_guard(_guard)
if(_guard)
    jafg_add_module(JAFG_MODULE_TYPE_SHARED)
    jafg_add_dependency("Lal/Lal" "public")
    jafg_add_dependency("Engine/EngineForward" "private")

    target_include_directories(${_scoped_module_name} PRIVATE
        "${JAFG_ENGINE_ROOT}/Engine/Vendor/Include"
        "${JAFG_ENGINE_ROOT}/Engine/Vendor/Include/Freetype"
        )

    if(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_LINUX)
        target_link_libraries(${_scoped_module_name} PRIVATE
            "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/libglfw3.a"
            "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/libfreetype.so"
            )
        if((JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG) OR (JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT))
            target_link_libraries(${_scoped_module_name} PRIVATE
                "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/libFastNoiseD.so"
                )
        elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
            target_link_libraries(${_scoped_module_name} PRIVATE
                "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/libFastNoise.a"
                )
        endif()
    elseif(JAFG_TARGET_PLATFORM STREQUAL JAFG_PLATFORM_WINDOWS)
        target_link_libraries(${_scoped_module_name} PRIVATE
            "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/glfw3.lib"
            "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/freetype.lib"
            )
        if((JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEBUG) OR (JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_DEVELOPMENT))
            target_link_libraries(${_scoped_module_name} PRIVATE
                "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/FastNoiseD.lib"
                )
        elseif(JAFG_TARGET_CONFIG STREQUAL JAFG_CONFIG_SHIPPING)
            target_link_libraries(${_scoped_module_name} PRIVATE
                "${JAFG_ENGINE_ROOT}/Engine/Vendor/Lib/FastNoise.lib"
                )
        endif()
    endif()
endif()
