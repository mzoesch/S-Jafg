// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES && PLATFORM_LINUX

#include "Foreign/Plugin.h"
#include "Foreign/PluginLifetime.h"
#include <dlfcn.h>

namespace Jafg
{

EPluginLoadReturnCode::Type LLoadedPlugin::OpenLibrary()
{
    check( this->IsValid() )

    this->Handle = dlopen(this->BinPath.ToPtr(), RTLD_LAZY);

    if (this->Handle == nullptr)
    {
        LOG_ERROR(LogForeign, "Failed to dlopen library: [{}].", this->BinPath)
        return EPluginLoadReturnCode::PlatformError;
    }

    dlerror();

    typedef LPluginLifetime* (*LCreatePluginLifetime)();

    const LString Symbol = LString::SprintF("GetPluginLifetime_{}", this->GetIdentifier());

    LCreatePluginLifetime CreatePluginLifetime = reinterpret_cast<LCreatePluginLifetime>(dlsym(this->Handle, Symbol.ToPtr()));
    const char* Error = dlerror();
    if (Error)
    {
        LOG_ERROR(LogForeign, "Failed to dlsym symbol: [{}].", Symbol)
        dlclose(this->Handle);
        this->Handle = nullptr;
        return EPluginLoadReturnCode::NoLifetimeHandle;
    }

    this->Lifetime = CreatePluginLifetime();
    if (this->Lifetime == nullptr)
    {
        LOG_ERROR(LogForeign, "Failed to create plugin lifetime.")
        dlclose(this->Handle);
        this->Handle = nullptr;
        return EPluginLoadReturnCode::NoLifetime;
    }

    this->Lifetime->OnStartup();

    return EPluginLoadReturnCode::Success;
}

void LLoadedPlugin::PrePareLibraryClose(const EPluginShutdownReason::Type InReason)
{
    if (this->IsLoaded() == false)
    {
        LOG_ERROR(LogForeign, "Plugin is not loaded.")
        return;
    }

    if (this->Lifetime != nullptr)
    {
        this->Lifetime->OnPrepareShutdown(InReason);
    }
    else
    {
        LOG_ERROR(LogForeign, "Lifetime is invalid.")
    }

    return;
}

EPluginLoadReturnCode::Type LLoadedPlugin::CloseLibrary(const EPluginShutdownReason::Type InReason)
{
    check( this->IsValid() )

    if (this->IsLoaded() == false)
    {
        return EPluginLoadReturnCode::NotLoaded;
    }

    if (this->Lifetime != nullptr)
    {
        this->Lifetime->OnShutdown(InReason);
        delete this->Lifetime;
        this->Lifetime = nullptr;
    }
    else
    {
        LOG_ERROR(LogForeign, "Lifetime is invalid.")
    }

    dlclose(this->Handle);
    this->Handle = nullptr;
    check( this->IsLoaded() == false )

    return EPluginLoadReturnCode::Success;
}

} /* ~Namespace Jafg */

#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES && PLATFORM_LINUX */
