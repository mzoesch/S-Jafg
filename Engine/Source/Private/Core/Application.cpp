// Copyright mzoesch. All rights reserved.

#include "Core/Application.h"
#include "Build/EngineBuildInfo.h"
#include "Core/Uuid.h"
#include "Stats/Stats.h"
#include "Runtime/Args.h"

namespace Llmm
{

void* Alloc(std::size_t Size, std::align_val_t) noexcept
{
    return std::malloc(Size);
}

void Free(void* Ptr) noexcept
{
    std::free(Ptr);
}

void* Realloc(void* Ptr, std::size_t NewSize, std::align_val_t) noexcept
{
    return std::realloc(Ptr, NewSize);
}

} /* ~Namespace Llmm */

namespace Jafg::Detail
{

ENGINE_API std::vector<LProgramParameter*> GRegisteredProgramArguments;

ENGINE_API std::mutex GLongLiquidLogsMutex;
/* This still exists even if LAL_SAVE_LOGS_IN_MEMORY but how would we access that macro here?? */
ENGINE_API std::vector<std::tuple<std::string, std::string>> GLongLiquidLogs;

} /* ~Namespace Jafg::Detail */

namespace Jafg::Application::Private
{

ENGINE_API bool bGAlreadyCrashed{};
ENGINE_API bool bGSuppressCrashDialog{};

} /* ~Namespace Jafg::Application::Private */

namespace
{

Jafg::LProgramParameter _CoreHelp{
    "Help",
    "Shows this help window for core systems only."
    };

Jafg::LProgramParameter _Help{
    "VerboseHelp",
    "Shows help window for all default loaded systems."
    };

Jafg::LProgramParameter _Version{
    "Version",
    "Shows the version of the engine."
    };

Jafg::LProgramParameter _version{
    "version",
    "Same as \"Version\" but in lowercase."
    };

Jafg::LProgramParameter _WaitForDebugger{
    "WaitForDebugger",
    "Whether to wait for a debugger to attach to this process before continuing execution.",
    };

Jafg::LProgramParameter _IgnoreInstantDebuggerBreak{
    "IgnoreInstantDebuggerBreak",
    "If \"-WaitForDebugger\" is specified, this will ignore the instant break after the debugger attached and continue execution normally.",
    };

Jafg::LProgramParameter _AllowProfiling{
    "AllowProfiling",
    "Whether to allow profiling and stats gathering.",
    };

Jafg::LProgramParameter _PauseBeforeExit{
    "PauseBeforeExit",
    "Whether to pause before exiting the application.",
    };

Jafg::LProgramParameter _AlwaysReportCrash{
    "AlwaysReportCrash",
    "Whether to always show crash dialog windows (if a front-end is available) and report them.",
    };

void API()
{
    u64 MaxSize { 0 };
    for (const auto& Param : Jafg::Detail::GRegisteredProgramArguments)
    {
        MaxSize = maths::max(MaxSize, static_cast<u64>(Param->Identifier.size()));
    }

    LOG_INFO(LogCli, "Available command line parameters:")
    for (const auto& Param : Jafg::Detail::GRegisteredProgramArguments)
    {
        LOG_INFO(LogCli, "  -{:<{}} : {}", Param->Identifier, MaxSize, Param->Description.c_str())
        continue;
    }

    return;
}

void APIVersion()
{
    LOG_INFO(LogCli, "Engine version [{}] @mzoesch at [{} - {}] on {} in {}.",
        Jafg::BuildInfo::GetEngineVersionStr(),
        Jafg::BuildInfo::GetBuildTime(),
        Jafg::BuildInfo::GetBuildDate(),
        Jafg::BuildInfo::GetVcsBranch(),
        Jafg::BuildInfo::GetVcsRevision()
        )

    return;
}

} /* ~Namespace <Anonymous> */

void Jafg::Hal::Sleep(const f64 InSeconds)
{
    STAT_CYCLE_FUNCTION()
    Jafg::Hal::SleepNoStats(InSeconds);
    return;
}

namespace Jafg::Private
{

ENGINE_API LUnderlyingUuidType CurrentUuidValue { 0 };

} /* ~Namespace Jafg::Private */

namespace Jafg::Application::Private
{

void WaitForDebuggerGracefully(const bool bAllowInstantBreak)
{
    LOG_INFO(LogJafgInternal, "Waiting for debugger ...");
    Jafg::FlushOutStreams();

    while (HasTracerPidNow() == false)
    {
        Jafg::Hal::SleepNoStats(1.0);
        continue;
    }

    LOG_INFO(LogJafgInternal, "Debugger attached - continuing.");
    Jafg::FlushOutStreams();

    if (bAllowInstantBreak)
    {
        if (algo::contains(Application::GetRawCmdLine(), "-IgnoreInstantDebuggerBreak") == false)
        {
            JAFG_PLATFORM_BREAK()
        }
    }

    return;
}

ENGINE_API TArray<LString> RawCommandLine;
ENGINE_API TArray<LProgramArgument> ProcessedCommandLine;

void ProcessCommandLineVariables()
{
    check( ProcessedCommandLine.empty() )

    LProgramArgument* List { nullptr };

    for (const auto& Arg : RawCommandLine)
    {
        if (Arg.starts_with('-'))
        {
            if (const auto Idx { Arg.find('=') }; Idx != Arg.npos)
            {
                ProcessedCommandLine.emplace_back(algo::sub(Arg, 1, Idx), algo::right_sub(Arg, Idx + 1));
                List = nullptr;
            }
            else
            {
                ProcessedCommandLine.emplace_back(algo::right_chop(Arg, 1));
                List = &ProcessedCommandLine.back();
            }
        }
        else
        {
            if (List)
            {
                check( List->Value.has_value() == false )
                if (List->Values.has_value() == false)
                {
                    List->Values.emplace();
                }
                List->Values->emplace_back(Arg);
            }
            else
            {
                ProcessedCommandLine.emplace_back(Arg);
            }
        }

        continue;
    }

    return;
}

std::tuple<bool, EPlatformExit::Type> ConditionallyShowHelpAndExit()
{
    if (HasCmdLineParameter("Help"))
    {
        ::API();
        return { true, EPlatformExit::Success };
    }

    return { false, EPlatformExit::Success };
}

std::tuple<bool, EPlatformExit::Type> ConditionallyShowVerboseHelpAndExit()
{
    if (HasCmdLineParameter("VerboseHelp"))
    {
        ::API();
        return { true, EPlatformExit::Success };
    }

    return { false, EPlatformExit::Success };
}

std::tuple<bool, EPlatformExit::Type> ConditionallyShowVersionAndExit()
{
    if (HasCmdLineParameter("Version") || HasCmdLineParameter("version"))
    {
        ::APIVersion();
        return { true, EPlatformExit::Success };
    }

    return { false, EPlatformExit::Success };
}

ENGINE_API bool    bPauseBeforeExit   { false };
ENGINE_API bool    bHasTracerPid      { false };
ENGINE_API bool    bAlwaysReportCrash { false };
ENGINE_API bool    bAllowProfiling    { false };

ENGINE_API f64 DeltaTime     { 0.0 };
ENGINE_API f64 RealDeltaTime { 0.0 };
ENGINE_API f64 LostDeltaTime { 0.0 };
ENGINE_API f64 IdleDeltaTime { 0.0 };
ENGINE_API u64 FrameCount    { 0   };

ENGINE_API f64 CurrentFrameTime  { 0.0 };
ENGINE_API f64 PreviousFrameTime { 0.0 };

ENGINE_API LHrcTimePoint StaticContainerInitializationTime { Hrc::now() };
ENGINE_API f64           LowestDeltaTime                   { std::numeric_limits<double>::max() };
ENGINE_API f64           HighestDeltaTime                  { -1.0 };
ENGINE_API f64           HighestLostDeltaTime              {  0.0 };
ENGINE_API f64           HighestIdleDeltaTime              {  0.0 };
ENGINE_API LHrcTimePoint PreviousStatisticsStartTime       { StaticContainerInitializationTime };
ENGINE_API u64           PreviousStatisticsFrameCount      { FrameCount };
ENGINE_API f64           PreviousLowestDeltaTime           { LowestDeltaTime };
ENGINE_API f64           PreviousHighestDeltaTime          { HighestDeltaTime };
ENGINE_API f64           PreviousHighestLostDeltaTime      { HighestLostDeltaTime };
ENGINE_API f64           PreviousHighestIdleDeltaTime      { HighestIdleDeltaTime };
ENGINE_API LHrcTimePoint LastStatisticsTime                { StaticContainerInitializationTime };
ENGINE_API u64           StatisticsFrameCount              { 0 };
ENGINE_API f32           StatisticsPeriod                  { 1.0f };

ENGINE_API LHrcTimePoint LastStdOutFlushTime;

} /* ~Namespace Jafg::Application::Private */

namespace Jafg::Application
{

bool HasCmdLineParameter(const LString& Parameter, LProgramArgument const** Out /* = nullptr */) noexcept
{
    for (const auto& Arg : Private::ProcessedCommandLine)
    {
        if (Arg.Identifier == Parameter)
        {
            if (Out)
            {
                *Out = &Arg;
            }

            return true;
        }

        continue;
    }

    return false;
}

bool HasTracerPidNow()
{
    Private::bHasTracerPid = Jafg::Hal::IsTracerPidValidVerySlow();
    return HasTracerPid();
}

bool CanEverProfile() noexcept
{
#if WITH_STATS
    return true;
#else /* WITH_STATS */
    return false;
#endif /* !WITH_STATS */
}

bool IsAllowProfiling() noexcept
{
#if WITH_STATS
    return Private::bAllowProfiling;
#else /* WITH_STATS */
    return false;
#endif /* !WITH_STATS */
}

} /* ~Namespace Jafg::Application */
