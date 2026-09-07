// Copyright mzoesch. All rights reserved.

#include "Core/App.h"
#include "Core/LaunchProgress.h"
#include "User/UserPreferences.h"
#include "Cli/ReSTCliPreferences.h"
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
    STAT_FUNCTION()

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

    Detail::EmitAndFlushLogs();

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
    if (App::GetCommandLineArgument(App::DisallowAnsi))
    {
        App::Detail::DisallowAnsi = true;
    }
    if (App::GetCommandLineArgument(App::Quiet))
    {
        Detail::GIsQuiet = true;
    }
    if (App::GetCommandLineArgument(App::EmitInformation))
    {
        Detail::GAllowInformation = true;
    }
    if (App::GetCommandLineArgument(App::CoreHelp))
    {
        Detail::GIsQuiet = true;
        Detail::GAllowHelp = true;
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Help shown.");
        return ::GetMostSignificantExitReason();
    }
    if (App::GetCommandLineArgument(App::Version))
    {
        Detail::GIsQuiet = true;
        Detail::GAllowHelp = true;
        App::PrettyPrintVersion();
        App::RequestEngineExit("Version shown.");
        return ::GetMostSignificantExitReason();
    }
    if (App::GetCommandLineArgument(App::Help))
    {
        Detail::GIsQuiet = true;
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

#if JAFG_WITH_STATS && JAFG_WITH_LOCAL_LAYER
    LOG_VERBOSE(LogInformation, "ProfilerExecutable={}", App::Detail::GetProfilerExecutable())
#endif /* JAFG_WITH_STATS && JAFG_WITH_LOCAL_LAYER */

    std::filesystem::current_path(finder::detail::_engine_root_dir_slow());
    finder::create_directories(finder::temp_dir());

    LOG_VERBOSE(LogLaunch, "Acquiring app lock.")
    if (auto Result{App::Detail::TryAcquireAppLock()}; Result == App::Detail::EAppLockResult::Unknown)
    {
        LOG_FATAL(LogLaunch, "Failed to acquire app lock.")
    }
    else if (Result == App::Detail::EAppLockResult::Shared)
    {
        // TODO: Allow this if we e.g. passed the owning process and enter a quiet mode where this
        //       instance makes it fs-requests to the parent. But for now we do not allow this.
        App::RequestEngineExit(1, "An instance of Jafg is already running.");
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_STATS
    if (App::GetCommandLineArgument(App::WaitForProfiler))
    {
#if JAFG_STATS_ON_DEMAND
        LOG_VERBOSE(LogLaunch, "Waiting for profiler to connect...")
        Detail::EmitAndFlushLogs();
        while (!tracy::GetProfiler().IsConnected())
        {
            App::SleepNoStats(0.5);
        }
#else /* JAFG_STATS_ON_DEMAND */
        LOG_FATAL(LogLaunch, "JAFG_STATS_ON_DEMAND is not enabled.")
#endif /* !JAFG_STATS_ON_DEMAND */
    }
#endif /* JAFG_WITH_STATS */
    STAT_FUNCTION()

    Tasks::RegisterThread(ENamedThreads::Master);
    finder::create_directories(finder::dumps_dir());
    finder::create_directories(finder::saved_dir());
    LOG_VERBOSE(LogInformation, "EngineDir={}", finder::detail::_engine_root_dir_slow())
    LOG_VERBOSE(LogInformation, "ProcDir={}", finder::detail::self_proc_dir_slow())

    /* This is technically a data race but who really cares. */
    finder::detail::dump_file = absolute(finder::most_recent_mem_dump_file());
    finder::create_directories(finder::detail::dump_file.parent_path());
    LOG_VERBOSE(LogInformation, "DumpFile={}", finder::detail::dump_file)

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

    LaunchProgress::PrepareBeginProgress();
    LaunchProgress::BeginProgress("Core Initialization", "Engine pre-life initialization", 0.0f);

    {
        STAT_ZONE("EngineInitialization")

        check(GEngine == nullptr)
        Detail::GMutableEngine = new LEngine{};
        GEngine = Detail::GMutableEngine;
        check(GEngine)
        if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::BeforeEngineInitButAfterAllocDangerous, Tasks::RunAllTasks);
        if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }

        {
            STAT_ZONE("ObjectInitialization")
            Detail::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(false);
            Detail::GetGlobalCxxRecordRegistry().LoadPendingPackages(LLoadedPluginHandle::GetEnginePluginHandle());
            if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
            Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterCorePackageLoadDangerous, Tasks::RunAllTasks);
            if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
        }

        Detail::GMutableEngine->Initialize();
        Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);
        if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    }

#if JAFG_WITH_FOREIGN_SUPPORT
    if (!App::GetCommandLineArgument(App::SkipForeignInit))
    {
        STAT_ZONE("ForeignInitialization")
        JUserPreferences const& Prefs{GetSingleton<JUserPreferences>()};
        Detail::GMutableEngine->RefetchPlugins(*Prefs.AdditionalPluginsSearchPaths);
        for (LString const& Plugin: *Prefs.EnabledEnginePlugins)
        {
            Detail::GMutableEngine->LoadPluginNoFailure(Plugin);
        }
        if (App::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    }
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    if (App::GetCommandLineArgument(App::Help))
    {
        check(Detail::GIsQuiet)
        Detail::GAllowHelp = true;
        App::PrettyPrintApiUsage();
        App::RequestEngineExit("Verbose help shown.");
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_REST_CLS
    {
        STAT_ZONE("ReSTCliInitialization")
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
    }
#endif /* JAFG_WITH_REST_CLS */

    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    LaunchProgress::FinishAndGiveUpMemory();

    if (App::GetCommandLineArgument(App::SkipLoop))
    {
        App::RequestEngineExit("SkipLoop requested.");
        return ::GetMostSignificantExitReason();
    }

    Detail::GMutableEngine->DefaultJumpStart();

#if JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    JAFG_PLATFORM_GUARDED_LOOP;
#else /* JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
    while (!App::IsTearingDown())
    {
        STAT_FRAME_MARK()
        STAT_ZONE("TickWrapper")

        check(GEngine)
        check(Tasks::IsOnMasterThread())

        App::Detail::BeginExitIfRequested();
        Detail::GMutableEngine->DefaultTimeAdvance();

        if (algo::time_diff(GEngine->LastStdoutFlush, GEngine->FrameStartTimePoint) > JAFG_FORCE_LOG_FLUSH_INTERVAL)
        {
            Tasks::Make(ENamedThreads::WorkerThread, ETaskTime::Whenever, &Detail::EmitAndFlushLogs);
            Detail::GMutableEngine->LastStdoutFlush = algo::now();
        }

        Detail::GMutableEngine->Tick();
        Detail::GetGlobalCarnifex().KillAllGarbageChildren();
    }
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
}
