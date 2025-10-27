// Copyright mzoesch. All rights reserved.

#include "Engine.afx"
#include "Core/LaunchProgress.h"
#include "User/UserPreferences.h"
#include "Platform/PlatformMisc.h"
#include "Cli/ReSTCliPreferences.h"

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

namespace
{

#if WITH_STATS
    //# The default tracer used in this program.
    Stats::LTracer RuntimeTracer;
#endif /* WITH_STATS */

} /* ~Namespace <Anonymous> */

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
    LEngine::_ReflectForwardedExitRequest();

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

    LEngine::_BeginExitIfRequested();

    {
        STAT_QUICK_CYCLE_START("UpdateTime")
        JUserPreferences const* UserPreferences{ GetDefault<JUserPreferences>() };

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
                if (Application::GetIdleDeltaTime() > Application::GetHighestIdleDeltaTime())
                {
                    Application::Private::HighestIdleDeltaTime = Application::GetIdleDeltaTime();
                }
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

    Private::GetGlobalCarnifex().KillAllGarbageChildren();

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
        LEngine::_ReflectForwardedExitRequest();
    }

    Private::GetGlobalCarnifex().KillAllGarbageChildren(); /* Non-CDRs */
    Private::GetGlobalCxxRecordRegistry().TearDown();
    Private::GetGlobalCarnifex().KillAllGarbageChildren(); /* CDRs */

    (void)Private::GetNameRegistry().Destroy();

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
    //# @see Motor/Launch.rs
    //#
    #include "Lal/Lal/Source/Test/TestLal.h"
#endif /* WITH_TESTS */

//# A function "guarded" by platform-specific code implementing error handlers and user interface crash reporters.
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

    if (auto const Ret{ Application::Private::ConditionallyShowHelpAndExit() }; std::get<0>(Ret))
    {
        GCustomExitStatusOverride = static_cast<i32>(std::get<1>(Ret));
        GCustomExitReason = "Help shown.";
        return ::GetMostSignificantExitReason();
    }

    if (auto const Ret{ Application::Private::ConditionallyShowVersionAndExit() }; std::get<0>(Ret))
    {
        GCustomExitStatusOverride = static_cast<i32>(std::get<1>(Ret));
        GCustomExitReason = "Version shown.";
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
#if WITH_STATS
    Application::Private::bAllowProfiling = Application::CanEverProfile() && Application::HasCmdLineParameter("AllowProfiling");
#endif /* WITH_STATS */

    Tasks::RegisterThread(ENamedThreads::Master);

#endif /* !WITH_TESTS */

    PlatformMisc::Private::InvalidateCachedValues();

#if PLATFORM_DESKTOP
    std::filesystem::current_path(PlatformMisc::GetEngineRootDir());
    Finder::CreateDirectories(Finder::GetSavedDir());
    Finder::CreateDirectories(Finder::GetSavesDir());
    Finder::CreateDirectories(Finder::GetDumpsDir());
#endif /* PLATFORM_DESKTOP */

    LOG_VERBOSE(LogSystem, "Engine root directory is [{}].", PlatformMisc::GetEngineRootDir())
    LOG_VERBOSE(LogSystem, "Real engine root directory is [{}].", PlatformMisc::GetRealEngineRootDir())

#if WITH_TESTS
    return Tester::LTestFramework{}.RunRegisteredTests();
#else /* WITH_TESTS  */

#if WITH_STATS
    if (Application::IsAllowProfiling())
    {
        LOG_VERBOSE(LogStats, "Profiling and stats gathering is enabled.")
        if (Stats::Private::GTracer == nullptr)
        {
            Stats::Private::GTracer = &::RuntimeTracer;
        }
        Stats::Private::GTracer->BeginSession("Program");
    }
#endif /* WITH_STATS */
    STAT_CYCLE_FUNCTION_START(GuardedMainCycle)
    STAT_BOOKMARK("GettingUp")

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    check( GEngine == nullptr )
    LEngine::PreInitialize();

    STAT_CYCLE_START(GmObjects, "JafgObjectInitialization")
    Private::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(false);
    Private::GetGlobalCxxRecordRegistry().LoadPendingPackages(LLoadedPluginHandle::GetEnginePluginHandle());
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
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::BeforeEngineInitButAfterAllocDangerous, Tasks::RunAllTasks);
    GEngine->Initialize();
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);

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
    const JUserPreferences* Prefs{ GetDefault<JUserPreferences>() };
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
    // LWorldStorage World = GEngine->SummonWorld("StartUpWorld");
    // GEngine->Browse(World, Name_LevelFrontend.ToString());
    STAT_CYCLE_END(GmEngineWorldLoad)

    checkSlow( GEngine )
    if (::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_REST_CLS
    checkSlow( GEngine )
    GEngine->SetReSTCliCorePaths();
    if (auto& ReSTCliPrefs{ *GetDefault<JReSTCliPreferences>() }; ReSTCliPrefs.bAlwaysDisable == false)
    {
        if
        (
               (ReSTCliPrefs.bAutoStart && !Application::HasCmdLineParameter("ReSTCli.DisableAutoStart"))
            || (Application::HasCmdLineParameter("ReSTCli.InstantStart"))
        )
        {
            GEngine->StartReSTCliServer();
        }
    }
    checkSlow( GEngine )
    if (::IsEngineExitRequested())
    {
        return ::GetMostSignificantExitReason();
    }
#endif /* JAFG_WITH_REST_CLS */

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
