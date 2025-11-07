// Copyright mzoesch. All rights reserved.

#include "Engine/EngineCompileTimeConstants.h"

#if JAFG_WITH_FOREIGN_SUPPORT && PLATFORM_LINUX

#include "Foreign/Plugin.h"
#include "Foreign/PluginLifetime.h"
#include "Engine/ClassOuter.h"
#include <dlfcn.h>

namespace Jafg
{

LLoadedPlugin::~LLoadedPlugin()
{
    if (this->IsLoaded())
    {
        //#
        //# If this ever triggers - we are fucked. The plugin probably has handles all over the place.
        //#
        LOG_WARNING(LogForeign, "Plugin [{}] was not closed before destruction.", this->GetIdentifier())
        this->CloseLibrary(EPluginShutdownReason::Unspecified);
    }

    return;
}

EPluginLoadReturnCode::Type LLoadedPlugin::OpenLibrary()
{
    check( Tasks::IsOnMasterThread() )

    check( this->IsValid() )
    check( this->IsLoaded() == false )

    this->NativeHandle = ::dlopen(this->BinPath.c_str()
        //
        // So this is now "now". We could change this to "lazy". This would be great for shipping configs as it would
        // not lead to chrashes if a plugin is missing symbols that are never used.
        // However, this only works for unix. Windows does not have an equivalent of lazy loading of symbols.
        //
        , RTLD_NOW

        // Also a cool flag. But windows does not have an equivalent.
        | RTLD_GLOBAL

        // Kinda cheecky but works.
        | (this->Fetched.bDynUnloadable ? 0 : RTLD_NODELETE)
        );

    if (char const* Error{ ::dlerror() })
    {
        LOG_ERROR(LogForeign, "Failed to dlopen library [{}]: {}.", this->BinPath, Error)
        if (this->NativeHandle)
        {
            ::dlclose(this->NativeHandle);
            this->NativeHandle = nullptr;
        }

        return EPluginLoadReturnCode::PlatformError;
    }

    if (this->NativeHandle == nullptr)
    {
        LOG_ERROR(LogForeign, "Failed to dlopen library [{}].", this->BinPath)
        return EPluginLoadReturnCode::PlatformError;
    }

    const LString Symbol{ Lal::SprintF("GetPluginLifetime_{}", this->GetIdentifier()) };

    typedef LPluginLifetime* (*LCreatePluginLifetime)();
    const LCreatePluginLifetime CreatePluginLifetime{ reinterpret_cast<LCreatePluginLifetime>(::dlsym(this->NativeHandle, Symbol.c_str())) };
    if (char const* Error{ ::dlerror() })
    {
        LOG_ERROR(LogForeign, "Failed to dlsym symbol [{}] in [{}]: {}.", Symbol, this->GetAbsolutePath(), Error)
        ::dlclose(this->NativeHandle);
        this->NativeHandle = nullptr;

        return EPluginLoadReturnCode::NoLifetimeHandle;
    }

    this->Lifetime = TUnique<LPluginLifetime>{CreatePluginLifetime()};
    if (this->Lifetime.get() == nullptr)
    {
        LOG_ERROR(LogForeign, "Failed to create plugin lifetime in [{}].", this->GetAbsolutePath())
        ::dlclose(this->NativeHandle);
        this->NativeHandle = nullptr;

        return EPluginLoadReturnCode::NoLifetime;
    }

    LOG_TRACE(LogForeign, "Waking up plugin lifetime for [{}].", this->GetIdentifier())
    this->Lifetime->OnStartup();

    return EPluginLoadReturnCode::Success;
}

void LLoadedPlugin::PrepareLibraryClose(const EPluginShutdownReason::Type InReason)
{
    check( Tasks::IsOnMasterThread() )

    if (this->IsLoaded() == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not loaded.", this->GetAbsolutePath())
        return;
    }

    if (this->Fetched.bDynUnloadable == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not marked as unloadable.", this->GetAbsolutePath())
        return;
    }

    if (this->Lifetime.get() != nullptr)
    {
        this->Lifetime->OnPrepareShutdown(InReason);
    }
    else
    {
        LOG_ERROR(LogForeign, "Lifetime [{}] is invalid.", this->GetAbsolutePath())
    }

    return;
}

EPluginLoadReturnCode::Type LLoadedPlugin::CloseLibrary(const EPluginShutdownReason::Type InReason)
{
    check( Tasks::IsOnMasterThread() )
    check( this->IsValid() )

    if (this->IsLoaded() == false)
    {
        check( this->NativeHandle == nullptr )
        return EPluginLoadReturnCode::NotLoaded;
    }

    if (this->Fetched.bDynUnloadable == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not marked as unloadable.", this->GetAbsolutePath())
        return EPluginLoadReturnCode::Failure;
    }

    check( this->NativeHandle )
    if (this->Lifetime.get() != nullptr)
    {
        this->Lifetime->OnShutdown(InReason);
        this->Lifetime.reset();
    }
    else
    {
        LOG_ERROR(LogForeign, "Lifetime in [{}] is invalid.", this->GetAbsolutePath())
    }

    ::dlclose(this->NativeHandle);
    this->NativeHandle = nullptr;
    check( this->IsLoaded() == false )

    return EPluginLoadReturnCode::Success;
}

} /* ~Namespace Jafg */

#endif /* JAFG_WITH_FOREIGN_SUPPORT && PLATFORM_LINUX */
