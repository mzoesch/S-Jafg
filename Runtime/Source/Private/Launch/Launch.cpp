// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Engine.h"
#include "Core/Application.h"
#include "Core/LaunchProgress.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/Carnifex.h"
#include "Platform/PlatformMisc.h"
#include "Async/TaskUtility.h"
#if WITH_VIRTUAL_FILESYSTEM
    #include "System/VFilesystem.h"
#endif /* WITH_VIRTUAL_FILESYSTEM */

using namespace Jafg;

namespace
{

LCarnifex* PrivateCarnifex = nullptr;

} /* ~Namespace <Anonymous> */

#if !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT) */
EPlatformExit::Type GetMostSignificantExitReason()
{
    /*
     * Even if the engine is null, this will not cause a crash as this member method must always behave in a static
     * way if itself is not defined.
     */
    GEngine->ReflectForwardedExitRequest();

    return ::HasCustomExitStatus()
        ? static_cast<EPlatformExit::Type>(::GetCustomExitStatus())
        : EPlatformExit::Success;
}

#if !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineTick()
{
    checkSlow( Jafg::Tasks::IsOnMasterThread() )

    LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST() /* Just temporary. */

    GEngine->BeginExitIfRequested();

    {
        GEngine->UpdateTime();
        GEngine->EnforceTickRate();
    }

    GEngine->Tick(Application::GetDeltaTimeAsFloat());

    PrivateCarnifex->KillAllGarbageChildren();

    return;
}

#if !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineExit()
{
    LOG_INFO(LogGuardedMain, "Engine is exiting ...")

    if (GEngine)
    {
        GEngine->TearDown();
        delete GEngine;
        GEngine = nullptr;
    }
    /*
     * Something very eccentric has happened or the pre-life-engine tasks failed to initialize before
     * the engine was even created.
     */
    else
    {
        /*
         * Even if the engine is null, this will not cause a crash as this member method must always behave in a static
         * way if itself is not defined.
         */
        GEngine->ReflectForwardedExitRequest();
    }

    if (GOmniVitaContext)
    {
        GOmniVitaContext->TearDownContext();
        delete GOmniVitaContext;
        GOmniVitaContext = nullptr;
    }

    if (PrivateCarnifex)
    {
        PrivateCarnifex->KillAllGarbageChildren();
        check( Private::GCarnifexReferrer == nullptr || PrivateCarnifex == *Private::GCarnifexReferrer )
        delete PrivateCarnifex;
        Private::GCarnifexReferrer = nullptr;
        PrivateCarnifex = nullptr;
    }

    if (Private::GObjectRegistry)
    {
        Private::KillSingletonObjectRegistry();
    }

#if WITH_VIRTUAL_FILESYSTEM
    if (GVirtualFileSystem)
    {
        delete GVirtualFileSystem;
        check( GVirtualFileSystem == nullptr )
    }
#endif /* WITH_VIRTUAL_FILESYSTEM */

    if (::HasCustomExitReason())
    {
        if ((::GetMostSignificantExitReason() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogGuardedMain, "Engine exit with custom exit reason: {}", ::GetCustomExitReason())
        }
        else
        {
            LOG_INFO(LogGuardedMain, "Engine exit with custom exit reason: {}", ::GetCustomExitReason())
        }
    }
    if (::HasCustomExitStatus())
    {
        if ((::GetCustomExitStatus() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogGuardedMain, "Engine exit with custom exit status: {}", ::GetCustomExitStatus())
        }
        else
        {
            LOG_INFO(LogGuardedMain, "Engine exit with custom exit status: {}", ::GetCustomExitStatus())
        }
    }

    return;
}

EPlatformExit::Type GuardedMain(const LChar* CmdLine)
{
#if !PLATFORM_USES_NON_GENERIC_EXIT
    struct GuardedMainScope
    {
        ~GuardedMainScope()
        {
            EngineExit();
        }
    } GuardedMainScope;
#endif /* !PLATFORM_USES_NON_GENERIC_EXIT */

    LOG_INFO(
        LogGuardedMain,
        "Finished static storage initialization after {} seconds.",
        Application::GetDeltaSinceStaticStorageInitialization()
    )

    Tasks::RegisterThread(ENamedThreads::Master);

#if WITH_VIRTUAL_FILESYSTEM
    new LVirtualFileSystem();
    if (GVirtualFileSystem == nullptr)
    {
        return EPlatformExit::Fatal;
    }
    LOG_INFO(LogSystem, "Found {} embedded files.", GVirtualFileSystem->GetTotalEmbeddedFileCount())
#endif /* WITH_VIRTUAL_FILESYSTEM */

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    PlatformMisc::InvalidateCachedValues();

    PrivateCarnifex            = new LCarnifex();
    Private::GCarnifexReferrer = &PrivateCarnifex;
    GOmniVitaContext           = new Private::LObjectContext();
    GOmniVitaContext->SetHumanReadableName("OmniVitaContext");
    check( GOmniVitaContext->GetCarnifex() )

    Private::CreateSingletonObjectRegistry();
    if (Private::GObjectRegistry == nullptr)
    {
        return EPlatformExit::Fatal;
    }
    Private::GObjectRegistry->LoadPendingPackages();

    if (GEngine)
    {
        return EPlatformExit::Fatal;
    }
    GEngine = new LEngine();
    GEngine->Initialize();

    if (::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }

    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    LaunchProgress::FinishAndGiveUpMemory();

#if PLATFORM_USES_NON_GENERIC_LOOP
    PLATFORM_GUARDED_LOOP;
#else /* PLATFORM_USES_NON_GENERIC_LOOP */
    while (::IsTearingDown() == false)
    {
        EngineTick();
    }
#endif /* !PLATFORM_USES_NON_GENERIC_LOOP */

    return GetMostSignificantExitReason();
}
