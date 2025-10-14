// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if JAFG_WITH_FOREIGN_SUPPORT

#include "Foreign/PluginLifetime.h"

#define DEFINE_PLUGIN(Api, InPluginLifetime, InPluginName)          \
    extern "C"                                                      \
    Api void LAL_JOIN_INNER_TWO(GetPluginLifetime_, InPluginName)() \
    {                                                               \
        new InPluginLifetime();                                     \
    }                                                               \
    PRIVATE_JAFG_PLUGINS_NATIVE_CALLS(InPluginLifetime)

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
    USE_WIN_MAIN_DLL - currently not implemented.

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
