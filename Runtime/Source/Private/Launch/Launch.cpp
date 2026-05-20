// Copyright mzoesch. All rights reserved.

#include "Core/App.h"
#include "Core/LaunchProgress.h"
#include "User/UserPreferences.h"
#include "Cli/ReSTCliPreferences.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "Engine/Jxx.h"

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

namespace
{

#if WITH_STATS
    //# The default tracer used in this program.
    Stats::LTracer RuntimeTracer;
#endif /* WITH_STATS */

} /* ~Namespace <Anonymous> */

/* Not a custom module. Place boilerplate manually. */
JAFG_LLMM_NEW_DEL_OPS_REPLACEMENTS

#if !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT) */
void FlushLogs()
{
    Jafg::FlushOutStreams();
    if (GEngine)
    {
        GMutableEngine->LastStdOutFlush = LEngine::Clock::now();
    }
    return;
}

#if !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT) */
EPlatformExit::Type GetMostSignificantExitReason()
{
    return App::HasCustomExitStatus()
        ? static_cast<EPlatformExit::Type>(App::GetCustomExitStatus())
        : EPlatformExit::Success;
}

#if !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineTick()
{
    STAT_CYCLE_FUNCTION()
    check(GEngine)
    check(Tasks::IsOnMasterThread())

    if (algo::time_diff(GEngine->LastStdOutFlush, std::chrono::high_resolution_clock::now()) > JAFG_FORCE_LOG_FLUSH_INTERVAL)
    {
        ::FlushLogs();
    }

    App::Detail::BeginExitIfRequested();
    GMutableEngine->DefaultTimeAdvance();
    GMutableEngine->Tick();
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    return;
}

#if !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT)
FORCEINLINE
#endif /* !(JAFG_PLATFORM_USES_NON_GENERIC_LOOP || JAFG_PLATFORM_USES_NON_GENERIC_EXIT) */
void EngineExit()
{
    STAT_BOOKMARK("TearingDown")
    STAT_CYCLE_FUNCTION_START(ExitCycle)

    LOG_INFO(LogLaunch, "Engine is exiting ...")

    if (GEngine)
    {
        GMutableEngine->TearDown();
    }

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    Detail::GetGlobalCxxRecordRegistry().TearDown();
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    if (GEngine)
    {
        delete GMutableEngine;
        GEngine = nullptr;
        GMutableEngine = nullptr;
    }

    (void)Detail::GetJxxTagRegistry().Destroy();

    if (App::HasCustomExitReason())
    {
        if ((::GetMostSignificantExitReason() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogLaunch, "Engine exit with custom exit reason: {}", App::GetCustomExitReason())
        }
        else
        {
            LOG_INFO(LogLaunch, "Engine exit with custom exit reason: {}", App::GetCustomExitReason())
        }
    }
    if (App::HasCustomExitStatus())
    {
        if ((App::GetCustomExitStatus() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogLaunch, "Engine exit with custom exit status: {}", App::GetCustomExitStatus())
        }
        else
        {
            LOG_INFO(LogLaunch, "Engine exit with custom exit status: {}", App::GetCustomExitStatus())
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

//# The launch function that is agnostic to all platforms. Each launch will eventually find its way here.
EPlatformExit::Type AgnosticLaunch()
{
    {
        check(App::Detail::ProcessedCommandLine.empty())
        LProgramArgument* CurrentList{};
        algo::for_each(App::Detail::RawCommandLine, [&CurrentList](LString const& Parameter)
        {
            if (Parameter.starts_with('-'))
            {
                CurrentList = nullptr;
                if (auto Idx{Parameter.find('=')}; Idx != LString::npos)
                {
                    App::Detail::ProcessedCommandLine.emplace_back(algo::sub(Parameter, 1, Idx), algo::right_chop(Parameter, Idx + 1));
                }
                else
                {
                    App::Detail::ProcessedCommandLine.emplace_back(algo::right_chop(Parameter, 1));
                    CurrentList = &App::Detail::ProcessedCommandLine.back();
                }
            }
            else
            {
                if (CurrentList)
                {
                    check(!CurrentList->IsValue())
                    if (CurrentList->IsStoreTrue())
                    {
                        CurrentList->Variant = TArray<LString>{};
                    }
                    std::get<TArray<LString>>(CurrentList->Variant).emplace_back(Parameter);
                }
                else
                {
                    LOG_WARNING(LogLaunch, "Command line argument [{}] is not associated with any parameter.", Parameter)
                }
            }
        });
    }

    if (App::GetCommandLineArgument(App::CoreHelp))
    {
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Help shown");
        return ::GetMostSignificantExitReason();
    }
    if (App::GetCommandLineArgument(App::Version))
    {
        App::PrettyPrintVersion();
        App::RequestEngineExit("Version shown");
        return ::GetMostSignificantExitReason();
    }

#if !WITH_TESTS
#if !JAFG_PLATFORM_USES_NON_GENERIC_EXIT
    algo::raii_leave _{&EngineExit};
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_EXIT */

    LOG_INFO(LogLaunch, "Finished static storage initialization after {} seconds.", App::GetElapsedTime())

    App::Detail::PauseBeforeExit = !!App::GetCommandLineArgument(App::PauseBeforeExit);
    App::Detail::AlwaysReportCrash = !!App::GetCommandLineArgument(App::AlwaysReportCrash);
#if WITH_STATS
    App::Detail::AllowProfiling = App::CanEverProfile() && !!App::GetCommandLineArgument(App::AllowProfiling);
#endif /* WITH_STATS */
#endif /* !WITH_TESTS */

    Tasks::RegisterThread(ENamedThreads::Master);

    std::filesystem::current_path(Finder::Detail::GetEngineRootDir());
    Finder::CreateDirectories(Finder::GetTempDir());
    Finder::CreateDirectories(Finder::GetDumpsDir());
    Finder::CreateDirectories(Finder::GetSavedDir());
    LOG_VERBOSE(LogSystem, "Engine root directory is [{}].", Finder::Detail::GetEngineRootDir())
    LOG_VERBOSE(LogSystem, "Real engine root directory is [{}].", Finder::Detail::GetSelfProcDir())

#if WITH_TESTS
    return Tester::LTestFramework{}.RunRegisteredTests();
#else /* WITH_TESTS  */

#if WITH_STATS
    if (App::IsAllowProfiling())
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

    STAT_CYCLE_START(AlEngineInit, "EngineInit")
    check(GEngine == nullptr)
    GMutableEngine = new LEngine{};
    GEngine = GMutableEngine;
    check(GEngine)
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::BeforeEngineInitButAfterAllocDangerous, Tasks::RunAllTasks);
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }

    STAT_CYCLE_START(AlObjects, "JafgObjectInitialization")
    Detail::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(false);
    Detail::GetGlobalCxxRecordRegistry().LoadPendingPackages(LLoadedPluginHandle::GetEnginePluginHandle());
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterCorePackageLoadDangerous, Tasks::RunAllTasks);
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlObjects)

    GMutableEngine->Initialize();
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlEngineInit)

#if JAFG_WITH_FOREIGN_SUPPORT
    STAT_CYCLE_START(AlEnabledEnginePluginsLoad, "EnabledEnginePluginsLoad")
    JUserPreferences const& Prefs{GetSingleton<JUserPreferences>()};
    GMutableEngine->RefetchPlugins(Prefs.AdditionalPluginsSearchPaths);
    for (LString const& Plugin : Prefs.EnabledEnginePlugins)
    {
        GMutableEngine->LoadPluginNoFailure(Plugin);
    }
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlEnabledEnginePluginsLoad)
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    if (App::GetCommandLineArgument(App::Help))
    {
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Verbose help shown");
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_REST_CLS
    STAT_CYCLE_START(AlReSTCliLoad, "ReSTCliLoad")
    GMutableEngine->SetReSTCliCorePaths();
    if (auto const& ReSTCliPrefs{GetSingleton<JReSTCliPreferences>()}; ReSTCliPrefs.bAlwaysDisable == false)
    {
        if ((ReSTCliPrefs.bAutoStart && !App::GetCommandLineArgument(Params::ReST_DisableAutoStart))
            || !!App::GetCommandLineArgument(Params::ReST_InstantStart))
        {
            GMutableEngine->StartReSTCliServer();
        }
    }
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlReSTCliLoad)
#endif /* JAFG_WITH_REST_CLS */

    FlushOutStreams();
    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    LaunchProgress::FinishAndGiveUpMemory();

    GMutableEngine->PreviousFrameTime = algo::time_diff(App::GetStaticStorageInitializationTime(), LEngine::Clock::now());
    Hal::YieldThread();
    GMutableEngine->FrameTime = algo::time_diff(App::GetStaticStorageInitializationTime(), LEngine::Clock::now());

    STAT_CYCLE_FUNCTION_END(GuardedMainCycle)
    STAT_BOOKMARK("GuardedMainCycle")

#if JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    JAFG_PLATFORM_GUARDED_LOOP;
#else /* JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
    while (!App::IsTearingDown())
    {
        ::EngineTick();
    }
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
#endif /* !WITH_TESTS */
}
