// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"
#include "Core/LaunchProgress.h"
#include "User/UserPreferences.h"
#include "Platform/PlatformMisc.h"
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
    return Application::HasCustomExitStatus()
        ? static_cast<EPlatformExit::Type>(Application::GetCustomExitStatus())
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

    Application::Detail::BeginExitIfRequested();
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

    LOG_INFO(LogGuardedMain, "Engine is exiting ...")

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

    if (Application::HasCustomExitReason())
    {
        if ((::GetMostSignificantExitReason() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogGuardedMain, "Engine exit with custom exit reason: {}", Application::GetCustomExitReason())
        }
        else
        {
            LOG_INFO(LogGuardedMain, "Engine exit with custom exit reason: {}", Application::GetCustomExitReason())
        }
    }
    if (Application::HasCustomExitStatus())
    {
        if ((Application::GetCustomExitStatus() & (EPlatformExit::Error | EPlatformExit::Fatal)) > 0)
        {
            LOG_ERROR(LogGuardedMain, "Engine exit with custom exit status: {}", Application::GetCustomExitStatus())
        }
        else
        {
            LOG_INFO(LogGuardedMain, "Engine exit with custom exit status: {}", Application::GetCustomExitStatus())
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

//# A function "guarded" by platform-specific code implementing error handlers and user interface crash reporters.
EPlatformExit::Type GuardedMain()
{
    {
        check(Application::Detail::ProcessedCommandLine.empty())
        LProgramArgument* CurrentList{};
        algo::for_each(Application::Detail::RawCommandLine, [&CurrentList](LString const& Parameter)
        {
            if (Parameter.starts_with('-'))
            {
                CurrentList = nullptr;
                if (auto Idx{Parameter.find('=')}; Idx != LString::npos)
                {
                    Application::Detail::ProcessedCommandLine.emplace_back(algo::sub(Parameter, 1, Idx), algo::right_chop(Parameter, Idx + 1));
                }
                else
                {
                    Application::Detail::ProcessedCommandLine.emplace_back(algo::right_chop(Parameter, 1));
                    CurrentList = &Application::Detail::ProcessedCommandLine.back();
                }
            }
            else
            {
                if (CurrentList)
                {
                    check(CurrentList->IsValue() == false)
                    if (CurrentList->IsStoreTrue())
                    {
                        CurrentList->Variant = TArray<LString>{};
                    }
                    std::get<TArray<LString>>(CurrentList->Variant).emplace_back(Parameter);
                }
                else
                {
                    LOG_WARNING(LogGuardedMain, "Command line argument [{}] is not associated with any parameter.", Parameter)
                }
            }
        });
    }

    if (Application::GetCommandLineArgument(Application::CoreHelp))
    {
        Application::PrettyPrintApiUsage();
        Application::RequestEngineExit("Help shown");
        return ::GetMostSignificantExitReason();
    }
    if (Application::GetCommandLineArgument(Application::Version))
    {
        Application::PrettyPrintVersion();
        Application::RequestEngineExit("Version shown");
        return ::GetMostSignificantExitReason();
    }

#if !WITH_TESTS
#if !JAFG_PLATFORM_USES_NON_GENERIC_EXIT
    algo::raii_leave _{&EngineExit};
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_EXIT */

    LOG_INFO(LogGuardedMain, "Finished static storage initialization after {} seconds.", Application::GetElapsedTime())

    Application::Detail::PauseBeforeExit = !!Application::GetCommandLineArgument(Application::PauseBeforeExit);
    Application::Detail::AlwaysReportCrash = !!Application::GetCommandLineArgument(Application::AlwaysReportCrash);
#if WITH_STATS
    Application::Detail::AllowProfiling = Application::CanEverProfile() && !!Application::GetCommandLineArgument(Application::AllowProfiling);
#endif /* WITH_STATS */
#endif /* !WITH_TESTS */

    Tasks::RegisterThread(ENamedThreads::Master);

    std::filesystem::current_path(PlatformMisc::GetEngineRootDir());
    Finder::CreateDirectories(Finder::GetTempDir());
    Finder::CreateDirectories(Finder::GetDumpsDir());
    Finder::CreateDirectories(Finder::GetSavedDir());

    LOG_VERBOSE(LogSystem, "Engine root directory is [{}].", PlatformMisc::GetEngineRootDir())
    LOG_VERBOSE(LogSystem, "Real engine root directory is [{}].", PlatformMisc::GetSelfProcDir())

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

    STAT_CYCLE_START(GmEngineInit, "EngineInit")
    check(GEngine == nullptr)
    GMutableEngine = new LEngine{};
    GEngine = GMutableEngine;
    check(GEngine)
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::BeforeEngineInitButAfterAllocDangerous, Tasks::RunAllTasks);
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }

    STAT_CYCLE_START(GmObjects, "JafgObjectInitialization")
    Detail::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(false);
    Detail::GetGlobalCxxRecordRegistry().LoadPendingPackages(LLoadedPluginHandle::GetEnginePluginHandle());
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterCorePackageLoadDangerous, Tasks::RunAllTasks);
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(GmObjects)

    GMutableEngine->Initialize();
    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::NoTickDangerous | ETaskTime::AfterEngineInitDangerous, Tasks::RunAllTasks);
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(GmEngineInit)

#if JAFG_WITH_FOREIGN_SUPPORT
    STAT_CYCLE_START(GmEnabledEnginePluginsLoad, "EnabledEnginePluginsLoad")
    JUserPreferences const& Prefs{GetSingleton<JUserPreferences>()};
    GMutableEngine->RefetchPlugins(Prefs.AdditionalPluginsSearchPaths);
    for (LString const& Plugin : Prefs.EnabledEnginePlugins)
    {
        GMutableEngine->LoadPluginNoFailure(Plugin);
    }
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(GmEnabledEnginePluginsLoad)
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    if (Application::GetCommandLineArgument(Application::Help))
    {
        Application::PrettyPrintApiUsage();
        Application::RequestEngineExit("Verbose help shown");
        return ::GetMostSignificantExitReason();
    }

#if JAFG_WITH_REST_CLS
    STAT_CYCLE_START(GmReSTCliLoad, "ReSTCliLoad")
    GMutableEngine->SetReSTCliCorePaths();
    if (auto const& ReSTCliPrefs{GetSingleton<JReSTCliPreferences>()}; ReSTCliPrefs.bAlwaysDisable == false)
    {
        if ((ReSTCliPrefs.bAutoStart && !Application::GetCommandLineArgument(Params::ReST_DisableAutoStart))
            || !!Application::GetCommandLineArgument(Params::ReST_InstantStart))
        {
            GMutableEngine->StartReSTCliServer();
        }
    }
    if (Application::IsEngineExitRequested()) { return ::GetMostSignificantExitReason(); }
    STAT_CYCLE_END(GmReSTCliLoad)
#endif /* JAFG_WITH_REST_CLS */

    FlushOutStreams();
    LaunchProgress::BeginProgress("End of initialization", "Starting ticking ...", 1.0f);
    LaunchProgress::FinishAndGiveUpMemory();

    GMutableEngine->PreviousFrameTime = algo::time_diff(Application::GetStaticStorageInitializationTime(), LEngine::Clock::now());
    Hal::YieldThread();
    GMutableEngine->FrameTime = algo::time_diff(Application::GetStaticStorageInitializationTime(), LEngine::Clock::now());

    STAT_CYCLE_FUNCTION_END(GuardedMainCycle)
    STAT_BOOKMARK("GuardedMainCycle")

#if JAFG_PLATFORM_USES_NON_GENERIC_LOOP
    JAFG_PLATFORM_GUARDED_LOOP;
#else /* JAFG_PLATFORM_USES_NON_GENERIC_LOOP */
    while (Application::IsTearingDown() == false)
    {
        ::EngineTick();
    }
#endif /* !JAFG_PLATFORM_USES_NON_GENERIC_LOOP */

    return ::GetMostSignificantExitReason();
#endif /* !WITH_TESTS */
}
