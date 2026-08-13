// Copyright mzoesch. All rights reserved.

#include "Core/App.h"
#include "Core/LaunchProgress.h"
#include "User/UserPreferences.h"
#include "Cli/ReSTCliPreferences.h"
#include "Stats/Stats.h"
#include "Engine/Engine.h"
#include "Engine/Jxx.h"

#if JAFG_WITH_TESTS
    #include "Core/Test.h"
#endif /* JAFG_WITH_TESTS */

using namespace Jafg;

#ifndef JAFG_FORCE_LOG_FLUSH_INTERVAL
    #if JAFG_IN_SHIPPING
        #define JAFG_FORCE_LOG_FLUSH_INTERVAL                           10.0
    #else /* JAFG_IN_SHIPPING */
        #define JAFG_FORCE_LOG_FLUSH_INTERVAL                           0.2
    #endif /* !JAFG_IN_SHIPPING */
#endif /* JAFG_FORCE_LOG_FLUSH_INTERVAL */

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
        Detail::GMutableEngine->LastStdOutFlush = algo::now();
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
void EngineExit()
{
    STAT_BOOKMARK("TearingDown")
    STAT_CYCLE_FUNCTION_START(ExitCycle)

    LOG_INFO(LogLaunch, "Engine is exiting ...")

    if (GEngine)
    {
        Detail::GMutableEngine->TearDown();
    }

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    Detail::GetGlobalCxxRecordRegistry().TearDown();
    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

    if (GEngine)
    {
        delete Detail::GMutableEngine;
        GEngine = nullptr;
        Detail::GMutableEngine = nullptr;
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
        auto Tracer{std::exchange(Stats::Private::GTracer, nullptr)};
        delete Tracer;
    }
#endif /* WITH_STATS */

    return;
}

//# The launch function that is agnostic to all platforms. Each launch will eventually find its way here.
EPlatformExit::Type AgnosticLaunch()
{
    LOG_INFO(LogLaunch, "Finished static storage initialization after [{}] seconds.", App::GetElapsedTime())

#if !JAFG_PLATFORM_USES_NON_GENERIC_EXIT
    algo::raii_leave _{&EngineExit};
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_EXIT */

    App::Detail::ProcessedCommandLine = App::ReprocessCommandLine(App::GetRawCommandLine());
    if (App::GetCommandLineArgument(App::CoreHelp))
    {
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Help shown.");
        return ::GetMostSignificantExitReason();
    }
    if (App::GetCommandLineArgument(App::Version))
    {
        App::PrettyPrintVersion();
        App::RequestEngineExit("Version shown.");
        return ::GetMostSignificantExitReason();
    }

    LOG_VERBOSE(LogInformation, "BuildTime={}", App::BuildTime())
    LOG_VERBOSE(LogInformation, "BuildDate={}", App::BuildDate())
    LOG_VERBOSE(LogInformation, "BuildVcsBranch={}", App::BuildVcsBranch())
    LOG_VERBOSE(LogInformation, "BuildVcsRevision={}", App::BuildVcsRevision())

    LOG_VERBOSE(LogInformation, "EngineVersion={}", App::EngineVersion())
    LOG_VERBOSE(LogInformation, "EngineVersionHash={}", App::EngineVersionHash())

    LOG_VERBOSE(LogInformation, "CompilerVersion={}", App::CompilerVersion())
    LOG_VERBOSE(LogInformation, "CxxStandard={}", App::CxxStandard())

    LOG_VERBOSE(LogInformation, "TargetPlatform={}", App::GetTargetPlatform())
    LOG_VERBOSE(LogInformation, "TargetArchitecture={}", App::GetTargetArchitecture())
    LOG_VERBOSE(LogInformation, "TargetType={}", App::GetTargetType())
    LOG_VERBOSE(LogInformation, "TargetConfiguration={}", App::GetTargetConfiguration())

    App::Detail::PauseBeforeExit = !!App::GetCommandLineArgument(App::PauseBeforeExit);
    LOG_VERBOSE(LogInformation, "PauseBeforeExit={}", App::Detail::PauseBeforeExit)
    App::Detail::AlwaysReportCrash = !!App::GetCommandLineArgument(App::AlwaysReportCrash);
    LOG_VERBOSE(LogInformation, "AlwaysReportCrash={}", App::Detail::AlwaysReportCrash)
    App::Detail::bDumpStack = !!App::GetCommandLineArgument(App::DumpStack);
    LOG_VERBOSE(LogInformation, "DumpStack={}", App::Detail::bDumpStack)
#if WITH_STATS
    App::Detail::AllowProfiling = App::CanEverProfile() && !!App::GetCommandLineArgument(App::AllowProfiling);
#endif /* WITH_STATS */
    LOG_VERBOSE(LogInformation, "AllowProfiling={}", App::IsAllowProfiling())

    std::filesystem::current_path(finder::detail::_engine_root_dir_slow());
    finder::create_directories(finder::temp_dir());

    LOG_VERBOSE(LogLaunch, "Acquiring app lock.")
    if (auto Result{App::Detail::TryAcquireAppLock()}; Result == App::Detail::EAppLockResult::Unknown)
    {
        LOG_FATAL(LogLaunch, "Failed to acquire app lock.")
    }
    else if (Result == App::Detail::EAppLockResult::Shared)
    {
        // TODO: Allow this if we e.g. passed the owning process and enter quite mode. But for now we do not allow this.
        App::RequestEngineExit(1, "An instance of Jafg is already running.");
        return ::GetMostSignificantExitReason();
    }

    Tasks::RegisterThread(ENamedThreads::Master);
    finder::create_directories(finder::dumps_dir());
    finder::create_directories(finder::saved_dir());
    LOG_VERBOSE(LogInformation, "EngineDir={}", finder::detail::_engine_root_dir_slow())
    LOG_VERBOSE(LogInformation, "ProcDir={}", finder::detail::self_proc_dir_slow())

    /* This is technically a data race but who really cares. */
    finder::detail::dump_file = absolute(finder::most_recent_mem_dump_file());
    finder::create_directories(finder::detail::dump_file.parent_path());
    LOG_VERBOSE(LogInformation, "DumpFile={}", finder::detail::dump_file)

#if WITH_STATS
    if (App::IsAllowProfiling())
    {
        LOG_VERBOSE(LogStats, "Profiling and stats gathering is enabled.")
        if (!Stats::Private::GTracer)
        {
            Stats::Private::GTracer = new Stats::LTracer{};
        }
        Stats::Private::GTracer->BeginSession("Program");
    }
#endif /* WITH_STATS */

#if JAFG_WITH_TESTS
    if (!App::GetCommandLineArgument(App::SkipTrivialTests))
    {
        if (auto Reason{LTestRunInstance{ETestCategoryBits::Trivial}.ExitReason()})
        {
            App::RequestEngineExit(EPlatformExit::Error, *Reason);
            return ::GetMostSignificantExitReason();
        }
    }
#endif /* JAFG_WITH_TESTS */

    STAT_CYCLE_FUNCTION_START(GuardedMainCycle)
    STAT_BOOKMARK("GettingUp")

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    STAT_CYCLE_START(AlEngineInit, "EngineInit")
    check(GEngine == nullptr)
    Detail::GMutableEngine = new LEngine{};
    GEngine = Detail::GMutableEngine;
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

    Detail::GMutableEngine->Initialize();
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlEngineInit)

#if JAFG_WITH_FOREIGN_SUPPORT
    STAT_CYCLE_START(AlEnabledEnginePluginsLoad, "EnabledEnginePluginsLoad")
    JUserPreferences const& Prefs{GetSingleton<JUserPreferences>()};
    Detail::GMutableEngine->RefetchPlugins(*Prefs.AdditionalPluginsSearchPaths);
    for (LString const& Plugin : *Prefs.EnabledEnginePlugins)
    {
        Detail::GMutableEngine->LoadPluginNoFailure(Plugin);
    }
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlEnabledEnginePluginsLoad)
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    if (App::GetCommandLineArgument(App::Help))
    {
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Verbose help shown.");
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_REST_CLS
    STAT_CYCLE_START(AlReSTCliLoad, "ReSTCliLoad")
    Detail::GMutableEngine->SetReSTCliCorePaths();
    if (auto const& ReSTCliPrefs{GetSingleton<JReSTCliPreferences>()}; ReSTCliPrefs.bAlwaysDisable == false)
    {
        if ((ReSTCliPrefs.bAutoStart && !App::GetCommandLineArgument(Params::ReST_DisableAutoStart))
            || !!App::GetCommandLineArgument(Params::ReST_InstantStart))
        {
            Detail::GMutableEngine->StartReSTCliServer();
        }
    }
    if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(AlReSTCliLoad)
#endif /* JAFG_WITH_REST_CLS */

    FlushOutStreams();
    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    LaunchProgress::FinishAndGiveUpMemory();

    Detail::GMutableEngine->FrameStartTimePoint = algo::now();
    std::this_thread::yield();
    Detail::GMutableEngine->FrameStartElapsedTime = App::GetElapsedTime(Detail::GMutableEngine->FrameStartTimePoint);
    Detail::GMutableEngine->DefaultTimeAdvance();

    STAT_CYCLE_FUNCTION_END(GuardedMainCycle)
    STAT_BOOKMARK("GuardedMainCycle")

#if JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    JAFG_PLATFORM_GUARDED_LOOP;
#else /* JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
    while (!App::IsTearingDown())
    {
        STAT_QUICK_CYCLE_START("TickWrapper")

        check(GEngine)
        check(Tasks::IsOnMasterThread())

        App::Detail::BeginExitIfRequested();
        Detail::GMutableEngine->DefaultTimeAdvance();

        if (algo::time_diff(GEngine->LastStdOutFlush, GEngine->FrameStartTimePoint) > JAFG_FORCE_LOG_FLUSH_INTERVAL)
        {
            ::FlushLogs();
        }

        Detail::GMutableEngine->Tick();
        Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    }
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
}
