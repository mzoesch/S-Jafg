// Copyright mzoesch. All rights reserved.

#include "Engine/EngineCompileTimeConstants.h"

#if JAFG_WITH_FOREIGN_SUPPORT && JAFG_PLATFORM_WINDOWS

#include "Foreign/Plugin.h"
#include "Foreign/PluginLifetime.h"
#include "Engine/Jxx.h"

namespace Jafg
{

EPluginLoadReturnCode::Type LLoadedPlugin::OpenLibrary()
{
    check( Tasks::IsOnMasterThread() )

    check( this->IsValid() )
    check( this->IsLoaded() == false )

    ::SetLastError(0);

    this->NativeHandle = ::LoadLibrary(this->BinPath.c_str());
    if (this->NativeHandle == nullptr)
    {
        const DWORD ErrorCode{ ::GetLastError() };
        LOG_ERROR(LogForeign, "Failed to LoadLibrary [{}]: Error Code {}.", this->BinPath, ErrorCode)

        return EPluginLoadReturnCode::PlatformError;
    }

    if (const DWORD ErrorCode{ ::GetLastError() }; ErrorCode != 0)
    {
        LOG_ERROR(LogForeign, "Failed to LoadLibrary [{}]: Error Code {}.", this->BinPath, ErrorCode)

        ::FreeLibrary(static_cast<HMODULE>(this->NativeHandle));
        this->NativeHandle = nullptr;

        return EPluginLoadReturnCode::PlatformError;
    }

    const LString Symbol{ algo::sprintf("GetPluginLifetime_{}", this->GetIdentifier()) };

    typedef LPluginLifetime* (*LCreatePluginLifetime)();
#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wcast-function-type-strict"
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wcast-function-type-strict"
#endif /* JAFG_WITH_GCC */
    const LCreatePluginLifetime CreatePluginLifetime{ reinterpret_cast<LCreatePluginLifetime>(::GetProcAddress(static_cast<HMODULE>(this->NativeHandle), Symbol.c_str())) };
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
#if JAFG_WITH_GCC
    #pragma GCC diagnostic pop
#endif /* JAFG_WITH_GCC */
    if (CreatePluginLifetime == nullptr)
    {
        const DWORD ErrorCode{ ::GetLastError() };
        LOG_ERROR(LogForeign, "Failed to GetProcAddress [{}] in [{}]: Error Code {}.", Symbol, this->BinPath, ErrorCode)
        ::FreeLibrary(static_cast<HMODULE>(this->NativeHandle));
        this->NativeHandle = nullptr;
        return EPluginLoadReturnCode::NoLifetimeHandle;
    }

    this->Lifetime = TUnique<LPluginLifetime>{CreatePluginLifetime()};
    if (this->Lifetime.get() == nullptr)
    {
        LOG_ERROR(LogForeign, "Failed to create plugin lifetime in [{}].", this->GetAbsolutePath())
        ::FreeLibrary(static_cast<HMODULE>(this->NativeHandle));
        this->NativeHandle = nullptr;

        return EPluginLoadReturnCode::NoLifetime;
    }

    LOG_TRACE(LogForeign, "Waking up plugin lifetime for [{}].", this->GetIdentifier())
    this->Lifetime->OnStartup();

    return EPluginLoadReturnCode::Success;
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

    ::FreeLibrary(static_cast<HMODULE>(this->NativeHandle));
    this->NativeHandle = nullptr;
    check( this->IsLoaded() == false )

    return EPluginLoadReturnCode::Success;
}

} /* ~Namespace Jafg */

#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES && JAFG_PLATFORM_WINDOWS */
