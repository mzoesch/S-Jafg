// Copyright mzoesch. All rights reserved.

#pragma once

#include "Foreign/PluginForward.h"

namespace Jafg
{

//#
//# This the main class that defines a foreign plugin.
//# Inherit from this and make it engine public by adding it to the DEFINE_PLUGIN macro.
//#
//# @see For questions, read this README.md: Foreign/README.md.
//#
class LPluginLifetime
{

public:

    virtual ~LPluginLifetime() = default;

    //#
    //# This function is called when the engine loads your plugin.
    //# Do your initialization here.
    //#
    virtual void OnStartup() { }

    //#
    //# This function is called before the #OnShutdown event is called.
    //# After this function all J-Objects of your plugin will be paraded before the butcher to be killed.
    //# Handle here objects that need to be manually removed.
    //#
    virtual void OnPrepareShutdown(const EPluginShutdownReason::Type InReason) { }

    //#
    //# This function is called when the engine unloads your plugin.
    //# @see #EPluginShutdownReason::Type on how to behave.
    //#
    virtual void OnShutdown(const EPluginShutdownReason::Type InReason) { }

    //#
    //# This function is called even before the #OnStartup and is called natively by the underlying
    //# platform.
    //# In some configurations or some platforms this may not be called or behave differently.
    //# Use with caution.
    //# Only do some very specific stuff here and may not be possible in the #OnStartup.
    //#
    static void OnNativeStartup() { }

    //#
    //# This function is called after the #OnShutdown and is called natively by the underlying
    //# platform.
    //# In some configurations or some platforms this may not be called or behave differently.
    //# Use with caution.
    //# Only do some very specific stuff here and may not be possible in the #OnShutdown.
    //#
    static void OnNativeShutdown() { }
};

} /* ~Namespace Jafg */
