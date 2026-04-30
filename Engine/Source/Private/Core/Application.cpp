// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"
#include "Core/Uuid.h"
#include "Stats/Stats.h"
#include "Runtime/Parameter.h"

namespace Jafg::Application
{

ENGINE_API LProgramParameter CoreHelp{{
    .Identifier = "Help",
    .Description = "Shows this help window for core systems only.",
    .Variations = {"h", "help"},
    }};
ENGINE_API LProgramParameter Version{{
    .Identifier = "Version",
    .Description = "Shows the version of the engine.",
    .Variations = {"v", "version"},
    }};

ENGINE_API LProgramParameter Help{{
    .Identifier = "Jafg.VerboseHelp",
    .Description = "Shows help window for all default loaded plugins.",
    }};

ENGINE_API LProgramParameter WaitForDebugger{{
    .Identifier = "Jafg.WaitForDebugger",
    .Description = "Whether to wait for a debugger to attach to this process before continuing execution.",
    }};
ENGINE_API LProgramParameter IgnoreInstantDebuggerBreak{{
    .Identifier = "Jafg.IgnoreInstantDebuggerBreak",
    .Description = "If \"-Jafg.WaitForDebugger\" is specified, this will ignore the instant break after the debugger attached and continue execution normally.",
    }};
ENGINE_API LProgramParameter AlwaysReportCrash{{
    .Identifier = "Jafg.AlwaysReportCrash",
    .Description = "Whether to always show crash dialog windows (if a front-end is available) and report them.",
    }};
ENGINE_API LProgramParameter AllowProfiling{{
    .Identifier = "Jafg.AllowProfiling",
    .Description = "Whether to allow profiling and stats gathering.",
    }};
ENGINE_API LProgramParameter PauseBeforeExit{{
    .Identifier = "Jafg.PauseBeforeExit",
    .Description = "Whether to pause before exiting the application.",
    }};

} /* ~Namespace Jafg::Application */

namespace Jafg::Application::Detail
{

ENGINE_API bool bAlreadyCrashed{};
ENGINE_API bool bSuppressCrashDialog{};

ENGINE_API bool bShouldRequestExit{};
ENGINE_API bool bEngineRequestingExit{};
ENGINE_API std::mutex ExitMutex;
ENGINE_API i32 CustomExitStatusOverride{ std::numeric_limits<i32>::max() };
ENGINE_API LString CustomExitReason;

ENGINE_API bool PauseBeforeExit{};
ENGINE_API bool IsTracerPidValid{};
ENGINE_API bool AlwaysReportCrash{};

#if WITH_STATS
    ENGINE_API bool AllowProfiling{};
#endif /* WITH_STATS */

ENGINE_API TArray<LString> RawCommandLine;
ENGINE_API TArray<LProgramArgument> ProcessedCommandLine;

ENGINE_API std::chrono::high_resolution_clock::time_point StaticContainerInitializationTime{ std::chrono::high_resolution_clock::now() };


} /* ~Namespace Jafg::Application::Detail */

namespace Jafg::Application::Detail
{

ENGINE_API TArray<LProgramParameter*> RegisteredProgramParameters;

} /* ~Namespace Jafg::Application::Detail */

namespace Jafg::Detail
{

ENGINE_API std::mutex GLongLiquidLogsMutex;
/* This still exists even if LAL_SAVE_LOGS_IN_MEMORY but how would we access that macro here?? */
ENGINE_API std::vector<std::tuple<std::string, std::string>> GLongLiquidLogs;


} /* ~Namespace Jafg::Detail */

namespace Jafg::Detail
{

ENGINE_API LUnderlyingUuidType CurrentUuidValue{};

} /* ~Namespace Jafg::Detail */

Jafg::LProgramParameter::~LProgramParameter()
{
    if (Application::WillShortlyTerminate() == false)
    {
        algo::erase(&Application::Detail::RegisteredProgramParameters, this);
    }
}

void Jafg::Application::Detail::WaitForDebuggerGracefully(bool bAllowInstantBreak)
{
    LOG_INFO(LogJafgInternal, "Waiting for debugger ...")
    Jafg::FlushOutStreams();

    while (Application::IsTracerPidValidNow() == false)
    {
        Hal::SleepNoStats(1.0);
    }

    LOG_INFO(LogJafgInternal, "Debugger attached - continuing.")
    Jafg::FlushOutStreams();

    if (bAllowInstantBreak)
    {
        if (algo::contains(Application::GetRawCommandLine(), "-Jafg.IgnoreInstantDebuggerBreak") == false)
        {
            JAFG_PLATFORM_BREAK()
        }
    }

    return;
}

void Jafg::Application::Detail::BeginExitIfRequested() noexcept
{
    check(Tasks::IsOnMasterThread())
    if (bShouldRequestExit)
    {
       bEngineRequestingExit = true;
    }
    return;
}

void Jafg::Application::RequestEngineExit() noexcept
{
    std::unique_lock Lock{Detail::ExitMutex};
    if (WillShortlyTerminate() == false)
    {
        Detail::bShouldRequestExit = true;
    }
}

void Jafg::Application::RequestEngineExit(LString Reason) noexcept
{
    std::unique_lock Lock{Detail::ExitMutex};
    if (WillShortlyTerminate() == false)
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitReason = std::move(Reason);
    }
}

void Jafg::Application::RequestEngineExit(i32 CustomExitStatus) noexcept
{
    std::unique_lock Lock{Detail::ExitMutex};
    if (WillShortlyTerminate() == false)
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitStatusOverride = CustomExitStatus;
    }
}

void Jafg::Application::RequestEngineExit(i32 CustomExitStatus, LString Reason) noexcept
{
    std::unique_lock Lock{Detail::ExitMutex};
    if (WillShortlyTerminate() == false)
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitStatusOverride = CustomExitStatus;
        Detail::CustomExitReason = std::move(Reason);
    }
}

void Jafg::Hal::Sleep(f64 InSeconds)
{
    STAT_CYCLE_FUNCTION()
    Hal::SleepNoStats(InSeconds);
}
