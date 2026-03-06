// Copyright mzoesch. All rights reserved.

#pragma once

#include "Engine/EngineCompileTimeConstants.h"

#if JAFG_WITH_FOREIGN_SUPPORT

#include "Foreign/PluginLifetime.h"

#define DEFINE_PLUGIN(Api, InPluginLifetime, InPluginName)          \
    extern "C"                                                      \
    Api void JAFG_JOIN_INNER_TWO(GetPluginLifetime_, InPluginName)() \
    {                                                               \
        new InPluginLifetime();                                     \
    }                                                               \
    PRIVATE_JAFG_PLUGINS_NATIVE_CALLS(InPluginLifetime)             \
    JAFG_LLMM_NEW_DEL_OPS_REPLACEMENTS

#define PRIVATE_JAFG_PLUGINS_LINUX_NATIVE_CALLS(InPluginLifetime) \
    __attribute__((constructor))                                  \
    void lib_init()                                               \
    {                                                             \
        InPluginLifetime::OnNativeStartup();                      \
    }                                                             \
    __attribute__((destructor))                                   \
    void lib_fini()                                               \
    {                                                             \
        InPluginLifetime::OnNativeShutdown();                     \
    }

#define PRIVATE_JAFG_PLUGINS_WIN_NATIVE_CALLS(InPluginLifetime) \
    BOOL APIENTRY DllMain(                                      \
        _In_ HMODULE hModule,                                   \
        _In_ DWORD   ul_reason_for_call,                        \
        _In_ LPVOID  lpReserved                                 \
        )                                                       \
    {                                                           \
        switch (ul_reason_for_call)                             \
        {                                                       \
        case DLL_PROCESS_ATTACH:                                \
        {                                                       \
            InPluginLifetime::OnNativeStartup();                \
            break;                                              \
        }                                                       \
        case DLL_PROCESS_DETACH:                                \
        {                                                       \
            if (lpReserved != nullptr)                          \
            {                                                   \
                /* do not do cleanup if process                 \
                   termination scenario */                      \
                break;                                          \
            }                                                   \
            InPluginLifetime::OnNativeShutdown();               \
            break;                                              \
        }                                                       \
        }                                                       \
                                                                \
        return TRUE;                                            \
    }

#if PLATFORM_LINUX
    #define PRIVATE_JAFG_PLUGINS_NATIVE_CALLS(InPluginLifetime)   \
        PRIVATE_JAFG_PLUGINS_LINUX_NATIVE_CALLS(InPluginLifetime)
#elif PLATFORM_WINDOWS
    #define PRIVATE_JAFG_PLUGINS_NATIVE_CALLS(InPluginLifetime) \
        PRIVATE_JAFG_PLUGINS_WIN_NATIVE_CALLS(InPluginLifetime)
#else
    #error "Unsupported platform."
#endif /* PLATFORM_LINUX */

#else /* JAFG_WITH_FOREIGN_SUPPORT */

#define DEFINE_PLUGIN(Api, InPluginLifetime, InPluginName) \
    Static insert to engine... not implemented

#endif /* !JAFG_WITH_FOREIGN_SUPPORT */
