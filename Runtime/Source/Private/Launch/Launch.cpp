// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Core/Application.h"
#include "Core/LaunchProgress.h"
#include "Core/Name.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/Carnifex.h"
#include "Platform/PlatformMisc.h"
#include "Async/TaskUtility.h"
#include "User/UserPreferences.h"
#if WITH_VIRTUAL_FILESYSTEM
    #include "System/VFilesystem.h"
#endif /* WITH_VIRTUAL_FILESYSTEM */

using namespace Jafg;

namespace
{

LCarnifex PrivateCarnifex;

} /* ~Namespace <Anonymous> */

#if IN_SHIPPING
    #define FORCE_LOG_FLUSH_INTERVAL        10.0
    #define LOG_TIME_FOR_VERY_LONG_FRAMES   2.0
#else /* IN_SHIPPING */
    #define FORCE_LOG_FLUSH_INTERVAL        0.2
    #define LOG_TIME_FOR_VERY_LONG_FRAMES   0.7
#endif /* !IN_SHIPPING */

FORCEINLINE void FlushLogs()
{
    LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST()
    Application::Private::LastStdOutFlushTime = Application::GetHighestNow();
    return;
}

#if !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(PLATFORM_USES_NON_GENERIC_LOOP || PLATFORM_USES_NON_GENERIC_EXIT) */
EPlatformExit::Type GetMostSignificantExitReason()
{
    //
    // Even if the engine is null, this will not cause a crash as this member method must always behave in a static
    // way if itself is not defined.
    //
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
    checkSlow( Tasks::IsOnMasterThread() )

    if (Application::GetTimeDiff(Application::Private::LastStdOutFlushTime, Application::GetHighestNow()) > FORCE_LOG_FLUSH_INTERVAL)
    {
        ::FlushLogs();
    }

    GEngine->BeginExitIfRequested();

    {
        const JUserPreferences* UserPreferences = GetDefault<JUserPreferences>();

        Application::Private::LostDeltaTime = 0.0;
        Application::Private::IdleDeltaTime = 0.0;

        const double ThisFrameTime = Application::GetTimeDifferenceFromStaticStorageInitialization(Application::GetHighestNow()) - Application::GetCurrentFrameTime();
        if (UserPreferences->bVSyncEnabled == false && UserPreferences->MaxFps != JUserPreferences::UnlimitedFps)
        {
            if (ThisFrameTime < 1.0 / UserPreferences->MaxFps)
            {
                const Application::LHrcTimePoint SleepStart = Application::GetHighestNow();
                const double SleepTime = (1.0 / UserPreferences->MaxFps) - ThisFrameTime;
                PlatformHal::Sleep(Maths::Max(SleepTime - 0.002, 0.0)); // This doesn't really work, sadly. How tf can we fix that - to sleep more precisely?
                Application::Private::IdleDeltaTime = Application::GetTimeDiff(SleepStart, Application::GetHighestNow());
            }
        }

        Application::Private::PreviousFrameTime = Application::GetCurrentFrameTime();
        Application::Private::CurrentFrameTime  = Application::GetTimeDifferenceFromStaticStorageInitialization(Application::GetHighestNow());
        Application::Private::DeltaTime         = Application::GetCurrentFrameTime() - Application::GetPreviousFrameTime();
        Application::Private::RealDeltaTime     = Application::GetDeltaTime();

        if (Application::GetDeltaTime() < Application::GetLowestDeltaTime())
        {
            Application::Private::LowestDeltaTime = Application::GetDeltaTime();
        }
        if (Application::GetDeltaTime() > Application::GetHighestDeltaTime())
        {
            Application::Private::HighestDeltaTime = Application::GetDeltaTime();
        }

        ++Application::Private::FrameCount;
        ++Application::Private::StatisticsFrameCount;

        if (Application::GetDeltaTime() > Application::MaxDeltaTime)
        {
            if constexpr (IS_COMPILED_LOG(LogGuardedMain, Warning))
            {
                if (Application::GetDeltaTime() > LOG_TIME_FOR_VERY_LONG_FRAMES)
                {
                    LOG_WARNING(LogGuardedMain, "Very long frame detected: {} seconds.", Application::GetDeltaTime())
                }
            }
            Application::Private::LostDeltaTime += Application::GetDeltaTime() - Application::MaxDeltaTime;
            if (Application::GetHighestLostDeltaTime() < Application::GetLostDeltaTime())
            {
                Application::Private::HighestLostDeltaTime = Application::GetLostDeltaTime();
            }
            Application::Private::DeltaTime = Application::MaxDeltaTime;
        }

        const Application::LHrcTimePoint CurrentSteadyTime = Application::GetHighestNow();
        if
        (
                std::chrono::duration<double>(CurrentSteadyTime - Application::GetLastStatisticsTime()).count()
            >
                Application::GetStatisticsPeriod()
        )
        {
            Application::ResetStatistics();
        }
    }

    GEngine->Tick(Application::GetDeltaTimeAsFloat());

    PrivateCarnifex.KillAllGarbageChildren();

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
    //
    // Something very eccentric has happened or the pre-life-engine tasks failed to initialize before
    // the engine was even created.
    //
    else
    {
        //
        // Even if the engine is null, this will not cause a crash as this member method must always behave in a static
        // way if itself is not defined.
        //
        GEngine->ReflectForwardedExitRequest();
    }

    if (GOmniVitaContext)
    {
        GOmniVitaContext->TearDownContext();
        delete GOmniVitaContext;
        GOmniVitaContext = nullptr;
    }

    PrivateCarnifex.KillAllGarbageChildren();
    Private::GCarnifexReferrer = nullptr;

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

    if (Private::GNameRegistry)
    {
        delete Private::GNameRegistry;
        Private::GNameRegistry = nullptr;
    }

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

EPlatformExit::Type GuardedMain()
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

    LOG_INFO
    (
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

    PlatformMisc::InvalidateCachedValues();

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    check( Private::GNameRegistry == nullptr )
    Private::GNameRegistry = new Private::LNameRegistry();
    LOG_VERBOSE(LogNames, "Program initialized {} names during static storage initialization.", Private::GetStaticNameCount())
    for (i32 Index = 0; Index < Private::GetStaticNameCount(); ++Index)
    {
        ensure( Private::GNameRegistry->RegisterName(Private::GetStaticNameByIndex(Index)) );
    }
    Private::ClearStaticNameContainer();
    LOG_INFO(LogNames, "Finished transferring static names to the name registry. With a total of {} names.", Private::GNameRegistry->GetNameCount())

    Private::GCarnifexReferrer = &PrivateCarnifex;
    GOmniVitaContext = new LObjectContext();
    GOmniVitaContext->DeferredInitialize(Private::GCarnifexReferrer);
    GOmniVitaContext->SetHumanReadableName("OmniVitaContext");
    check( GOmniVitaContext->GetCarnifex() )

    Private::CreateSingletonObjectRegistry();
    if (Private::GObjectRegistry == nullptr)
    {
        return EPlatformExit::Fatal;
    }
    Private::GObjectRegistry->LoadPendingPackages();
    if (::IsEngineExitRequested() || GEngine)
    {
        return EPlatformExit::Fatal;
    }

    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterCorePackageLoadDangerous, Tasks::Private::RunAllTasks);
    if (::IsEngineExitRequested() || GEngine)
    {
        return EPlatformExit::Fatal;
    }

    GEngine = new LEngine();
    GEngine->Initialize();

    if (GEngine == nullptr || ::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }

    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    ::FlushLogs();
    LaunchProgress::FinishAndGiveUpMemory();

#if PLATFORM_USES_NON_GENERIC_LOOP
    PLATFORM_GUARDED_LOOP;
#else /* PLATFORM_USES_NON_GENERIC_LOOP */
    while (::IsTearingDown() == false)
    {
        ::EngineTick();
    }
#endif /* !PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
}
