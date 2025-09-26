// Copyright mzoesch. All rights reserved.

#include "Lal.afx"
#include "Engine/EngineCompileTimeConstants.h"
#include "Engine/Engine.h"
#include "Core/Application.h"
#include "Core/LaunchProgress.h"
#include "Core/Name.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/Carnifex.h"
#include "Platform/PlatformMisc.h"
#include "Async/TaskUtility.h"
#include "Core/CoreNames.h"
#include "User/UserPreferences.h"
#include "Stats/Stats.h"
#if WITH_TESTS
    #include "TestCore/TestRunner.h"
#endif /* WITH_TESTS */

using namespace Jafg;

#ifndef JAFG_FORCE_LOG_FLUSH_INTERVAL
    #if IN_SHIPPING
        #define JAFG_FORCE_LOG_FLUSH_INTERVAL                           10.0
    #else /* IN_SHIPPING */
        #define JAFG_FORCE_LOG_FLUSH_INTERVAL                           0.2
    #endif /* !IN_SHIPPING */
#endif /* JAFG_FORCE_LOG_FLUSH_INTERVAL */

#ifndef JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES
    #if IN_SHIPPING
        #define JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES                      2.0
    #else /* IN_SHIPPING */
        #define JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES                      0.7
    #endif /* !IN_SHIPPING */
#endif /* JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES */

#include "Runtime/Args.h"

namespace
{

LCarnifex PrivateCarnifex;

#if WITH_STATS
    Stats::LTracer PrivateTracer;
#endif /* WITH_STATS */

} /* ~Namespace <Anonymous> */

namespace Jafg
{

struct LPrivateLaunch
{
    static void CreateGOmniVitaContext()
    {
        check( GOmniVitaContext == nullptr )
        GOmniVitaContext = new LObjectContext(LObjectContext::NoEngineRegistration);
    }

    static void DestroyGOmniVitaContext()
    {
        check( GOmniVitaContext )
        GOmniVitaContext->TearDownContextNoEngineUnregistration();
    }
};

} /* ~Namespace Jafg */

#if !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT) */
void FlushLogs()
{
    LAL_UNSAFE_FLUSH_OUT_STREAMS()
    Application::Private::LastStdOutFlushTime = Application::GetHighestNow();
    return;
}

#if !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT) */
EPlatformExit::Type GetMostSignificantExitReason()
{
    LEngine::ReflectForwardedExitRequest();

    return ::HasCustomExitStatus()
        ? static_cast<EPlatformExit::Type>(::GetCustomExitStatus())
        : EPlatformExit::Success;
}

#if !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineTick()
{
    STAT_CYCLE_FUNCTION()

    checkSlow( Tasks::IsOnMasterThread() )

    if (Application::GetTimeDiff(Application::Private::LastStdOutFlushTime, Application::GetHighestNow()) > JAFG_FORCE_LOG_FLUSH_INTERVAL)
    {
        ::FlushLogs();
    }

    LEngine::BeginExitIfRequested();

    {
        STAT_QUICK_CYCLE_START("UpdateTime")
        const JUserPreferences* UserPreferences = GetDefault<JUserPreferences>();

        Application::Private::LostDeltaTime = 0.0;
        Application::Private::IdleDeltaTime = 0.0;

        const f64 ThisFrameTime = Application::GetTimeDifferenceFromStaticStorageInitialization(Application::GetHighestNow()) - Application::GetCurrentFrameTime();
        if (UserPreferences->bVSyncEnabled == false && UserPreferences->MaxFps != JUserPreferences::UnlimitedFps)
        {
            if (ThisFrameTime < 1.0 / UserPreferences->MaxFps)
            {
                const Application::LHrcTimePoint SleepStart = Application::GetHighestNow();
                const f64 SleepTime = (1.0 / UserPreferences->MaxFps) - ThisFrameTime;
                Lal::Hal::SleepNoStats(Maths::Max(SleepTime - 0.002, 0.0)); // This doesn't really work, sadly. How tf can we fix that - to sleep more precisely?
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
                if (Application::GetDeltaTime() > JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES)
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

#if !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(LAL_PLATFORM_USES_NON_GENERIC_LOOP || LAL_PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineExit()
{
    STAT_BOOKMARK("TearingDown")
    STAT_CYCLE_FUNCTION_START(ExitCycle)

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
        LEngine::ReflectForwardedExitRequest();
    }

    if (GOmniVitaContext)
    {
        LPrivateLaunch::DestroyGOmniVitaContext();
        delete GOmniVitaContext;
        GOmniVitaContext = nullptr;
    }

    PrivateCarnifex.KillAllGarbageChildren();
    Private::GCarnifexReferrer = nullptr;

    if (Private::GObjectRegistry)
    {
        Private::KillSingletonObjectRegistry();
    }

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

    STAT_CYCLE_FUNCTION_END(ExitCycle)

#if WITH_STATS
    if (Stats::Private::GTracer)
    {
        Stats::Private::GTracer->EndSession();
        Stats::Private::GTracer = nullptr;
    }
#endif /* WITH_STATS */

    return;
}

#if WITH_TESTS
    //#
    //# We include the static library tests from LAL directly here, so that the linker will not flag them as
    //# unused and remove them during the linking phase to this runtime executable.
    //# We can do this because LAL does not use the default automatic simple unit test registration process.
    //#
    //# @see Motor/Launch.rs:80
    //#
    #include "Lal/Lal/Source/Test/TestLal.h"
#endif /* WITH_TESTS */

//#
//# A function that is "guarded" by platform-specific code implementing error handlers and user interface
//# crash reporters.
//#
EPlatformExit::Type GuardedMain()
{
#if !WITH_TESTS

#if !LAL_PLATFORM_USES_NON_GENERIC_EXIT
    struct GuardedMainScope
    {
        ~GuardedMainScope()
        {
            EngineExit();
        }
    } GuardedMainScope;
#endif /* !LAL_PLATFORM_USES_NON_GENERIC_EXIT */

    Application::Private::ProcessCommandLineVariables();

    if (const std::tuple Ret { Application::Private::ConditionallyShowHelpAndExit() }; std::get<0>(Ret))
    {
        GCustomExitStatusOverride = static_cast<i32>(std::get<1>(Ret));
        GCustomExitReason = "Help shown.";
        return ::GetMostSignificantExitReason();
    }

    LOG_INFO
    (
        LogGuardedMain,
        "Finished static storage initialization after {} seconds.",
        Application::GetDeltaSinceStaticStorageInitialization()
    )

    Application::Private::bPauseBeforeExit = Application::HasCmdLineParameter("PauseBeforeExit");
    Application::Private::bAlwaysReportCrash = Application::HasCmdLineParameter("AlwaysReportCrash");

    Tasks::RegisterThread(ENamedThreads::Master);

#endif /* !WITH_TESTS */

    PlatformMisc::Private::InvalidateCachedValues();

#if PLATFORM_DESKTOP
    std::filesystem::current_path(PlatformMisc::GetEngineRootDir().ToPtr());
    Finder::CreateDirectories(Finder::GetSavesDir());
#endif /* PLATFORM_DESKTOP */

    LOG_VERBOSE(LogSystem, "Engine root directory is [{}].", PlatformMisc::GetEngineRootDir())
    LOG_VERBOSE(LogSystem, "Real engine root directory is [{}].", PlatformMisc::GetRealEngineRootDir())

#if WITH_TESTS
    return Tester::LTestFramework{}.RunRegisteredTests();
#else /* WITH_TESTS  */

#if WITH_STATS
    if (Application::IsAllowProfiling())
    {
        Stats::Private::GTracer = &::PrivateTracer;
        Stats::Private::GTracer->BeginSession("Program");
    }
#endif /* WITH_STATS */
    STAT_CYCLE_FUNCTION_START(GuardedMainCycle)
    STAT_BOOKMARK("GettingUp")

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    check( GEngine == nullptr )
    LEngine::PreInitialize();

    STAT_CYCLE_START(GmNames, "StaticNameRegistration")
    check( Private::GNameRegistry == nullptr )
    Private::GNameRegistry = new Private::LNameRegistry();
    LOG_VERBOSE(LogNames, "Program initialized {} names during static storage initialization.", Private::GetStaticNameCount())
    for (i32 Index = 0; Index < Private::GetStaticNameCount(); ++Index)
    {
        ensure( Private::GNameRegistry->RegisterName(Private::GetStaticNameByIndex(Index)) );
    }
    Private::ClearStaticNameContainer();
    LOG_INFO(LogNames, "Finished transferring static names to the name registry. With a total of {} names.", Private::GNameRegistry->GetNameCount())
    STAT_CYCLE_END(GmNames)

    STAT_CYCLE_START(GmObjects, "JafgObjectInitialization")
    Private::GCarnifexReferrer = &::PrivateCarnifex;
    LPrivateLaunch::CreateGOmniVitaContext();
    check( GOmniVitaContext->IsValid() )
    GOmniVitaContext->SetHumanReadableName("OmniVitaContext");
    check( GOmniVitaContext->GetCarnifex() )

    Private::CreateSingletonObjectRegistry();
    if (Private::GObjectRegistry == nullptr)
    {
        return EPlatformExit::Fatal;
    }
    Private::GObjectRegistry->LoadPendingPackages(LLoadedPluginHandle::GetEnginePluginHandle());
    if (::IsEngineExitRequested() || GEngine)
    {
        return EPlatformExit::Fatal;
    }

    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterCorePackageLoadDangerous, Tasks::RunAllTasks);
    if (::IsEngineExitRequested() || GEngine)
    {
        return EPlatformExit::Fatal;
    }
    STAT_CYCLE_END(GmObjects)

    STAT_CYCLE_START(GmEngineInit, "EngineInit")
    GEngine = new LEngine();
    GEngine->RegisterObjectContext(GOmniVitaContext);
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::BeforeEngineInitButAfterAllocDangerous, Tasks::RunAllTasks);
    GEngine->Initialize();
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);

    //
    // The core levels. Hardcoded into the engine generation for better communication with other plugins.
    // To give them a common / default way for different engine states.
    //
    GEngine->RegisterLevel
    (
        LLevel
        {
            Name_LevelFrontend.ToString(), EInputMode::UserInterface, true
        }
    );
    GEngine->RegisterLevel
    (
        LLevel
        {
            Name_LevelMyWorld.ToString(), EInputMode::InputSubSystem, false,
            Lal::LLinearColor::Black,
            true, false, TArray<LLevelSkyboxMap>
            {
                LLevelSkyboxMap
                {
                    "Night",
                    TArray<LEnginePath>
                    {
                        LEnginePath{ EEnginePaths::Textures, "Misc/SbNight.png" },
                    },
                },
            },
        }
    );

    STAT_CYCLE_END(GmEngineInit)

    if (GEngine == nullptr || ::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_FOREIGN_SUPPORT
    STAT_CYCLE_START(GmEnabledEnginePluginsLoad, "EnabledEnginePluginsLoad")
    const JUserPreferences* Prefs { GetDefault<JUserPreferences>() };
    GEngine->RefetchPlugins(Prefs->AdditionalPluginsSearchPaths);
    for (const LString& Plugin : Prefs->EnabledEnginePlugins)
    {
        GEngine->LoadPluginNoFailure(Plugin);
        continue;
    }
    STAT_CYCLE_END(GmEnabledEnginePluginsLoad)

    if (GEngine == nullptr || ::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    STAT_CYCLE_START(GmEngineWorldLoad, "EngineWorldLoad")
    LWorldStorage World = GEngine->SummonWorld("StartUpWorld");
    GEngine->Browse(World, Name_LevelFrontend.ToString());
    STAT_CYCLE_END(GmEngineWorldLoad)

    if (GEngine == nullptr || ::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }

    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    ::FlushLogs();
    LaunchProgress::FinishAndGiveUpMemory();

    Application::Private::PreviousFrameTime = Application::GetTimeDifferenceFromStaticStorageInitialization(Application::GetHighestNow());
    Lal::Hal::YieldThread();
    Application::Private::CurrentFrameTime  = Application::GetTimeDifferenceFromStaticStorageInitialization(Application::GetHighestNow());

    STAT_CYCLE_FUNCTION_END(GuardedMainCycle)
    STAT_BOOKMARK("GuardedMainCycle")

#if LAL_PLATFORM_USES_NON_GENERIC_LOOP
    LAL_PLATFORM_GUARDED_LOOP;
#else /* LAL_PLATFORM_USES_NON_GENERIC_LOOP */
    while (::IsTearingDown() == false)
    {
        ::EngineTick();
    }
#endif /* !LAL_PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
#endif /* !WITH_TESTS */
}
