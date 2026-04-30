// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Arguments.h"
#include "Runtime/Parameter.h"
#include "Runtime/Argument.h"
#include "Stats/StatsForward.h"
#include "Build/EngineBuildInfo.h"

namespace Jafg
{

namespace Application
{

ENGINE_API extern LProgramParameter CoreHelp;
ENGINE_API extern LProgramParameter Version;
ENGINE_API extern LProgramParameter Help;
ENGINE_API extern LProgramParameter WaitForDebugger;
ENGINE_API extern LProgramParameter IgnoreInstantDebuggerBreak;
ENGINE_API extern LProgramParameter AlwaysReportCrash;
ENGINE_API extern LProgramParameter AllowProfiling;
ENGINE_API extern LProgramParameter PauseBeforeExit;

namespace Detail
{

ENGINE_API extern bool bAlreadyCrashed;
//# Not supported in all configurations.
ENGINE_API extern bool bSuppressCrashDialog;
ENGINE_API void WaitForDebuggerGracefully(bool bAllowInstantBreak);

//# Whether the engine should exit at the next opportunity.
ENGINE_API extern bool bShouldRequestExit;
//# Whether the engine has successfully received an exit request and is now beginning to tear down.
ENGINE_API extern bool bEngineRequestingExit;
ENGINE_API extern std::mutex ExitMutex;
ENGINE_API extern i32 CustomExitStatusOverride;
ENGINE_API extern LString CustomExitReason;
//# Called at the beginning of a frame by the engine. This will be the last frame.
ENGINE_API void BeginExitIfRequested() noexcept;

//# Not all platforms respect this.
ENGINE_API extern bool PauseBeforeExit;
ENGINE_API extern bool IsTracerPidValid;
ENGINE_API extern bool AlwaysReportCrash;

#if WITH_STATS
    ENGINE_API extern bool AllowProfiling;
#endif /* WITH_STATS */

//#
//# The command line. A parameter is defined as the following:
//#   -parameter
//#   -parameter=value                  (no spaces allowed)
//#   -parameter="string value"         (escape " with \)
//#   -parameter item item item         (no - allowed on items)
//#
ENGINE_API extern TArray<LString> RawCommandLine;
ENGINE_API extern TArray<LProgramArgument> ProcessedCommandLine;

ENGINE_API extern std::chrono::high_resolution_clock::time_point StaticContainerInitializationTime;

} /* ~Namespace Detail */

FORCEINLINE constexpr bool IsEngineExitRequested() noexcept { return Detail::bShouldRequestExit; }
FORCEINLINE constexpr bool IsTearingDown() noexcept { return Detail::bEngineRequestingExit; }
FORCEINLINE constexpr bool WillShortlyTerminate() noexcept { return IsEngineExitRequested() || IsTearingDown(); }

FORCEINLINE constexpr bool HasCustomExitStatus() noexcept { return Detail::CustomExitStatusOverride != std::numeric_limits<i32>::max(); }
FORCEINLINE constexpr i32  GetCustomExitStatus() noexcept { return Detail::CustomExitStatusOverride; }
FORCEINLINE constexpr bool HasCustomExitReason() noexcept { return Detail::CustomExitReason.empty() == false; }
FORCEINLINE constexpr LString GetCustomExitReason() noexcept { return Detail::CustomExitReason; }

ENGINE_API void RequestEngineExit() noexcept;
ENGINE_API void RequestEngineExit(LString Reason) noexcept;
ENGINE_API void RequestEngineExit(i32 CustomExitStatus) noexcept;
ENGINE_API void RequestEngineExit(i32 CustomExitStatus, LString Reason) noexcept;

FORCEINLINE constexpr bool IsPauseBeforeExit() noexcept { return Detail::PauseBeforeExit; }
FORCEINLINE constexpr bool IsTracerPidValid() noexcept { return Detail::IsTracerPidValid; }
FORCEINLINE bool IsTracerPidValidNow()
{
    Detail::IsTracerPidValid = Hal::IsTracerPidValidVerySlow();
    return IsTracerPidValid();
}
FORCEINLINE constexpr bool IsAlwaysReportCrash() noexcept { return Detail::AlwaysReportCrash; }

FORCEINLINE constexpr bool CanEverProfile() noexcept
{
#if WITH_STATS
    return true;
#else /* WITH_STATS */
    return false;
#endif /* !WITH_STATS */
}
FORCEINLINE constexpr bool IsAllowProfiling() noexcept
{
#if WITH_STATS
    return Detail::AllowProfiling;
#else /* WITH_STATS */
    return false;
#endif /* !WITH_STATS */
}

FORCEINLINE TArray<LString> const& GetRawCommandLine() noexcept { return Detail::RawCommandLine; }
FORCEINLINE TArray<LProgramArgument> const& GetCommandLine() noexcept { return Detail::ProcessedCommandLine; }

//#
//# Whether the command line contains the given argument.
//# @note This function will not work during static storage initialization or early program startup.
//#
inline LProgramArgument const* GetCommandLineArgument(LStringView Parameter) noexcept
{
    return algo::find_pointer(Detail::ProcessedCommandLine, Parameter, &LProgramArgument::Identifier);
}
inline LProgramArgument const* GetCommandLineArgument(LProgramParameter const& Parameter) noexcept
{
    LProgramArgument const* Argument{algo::find_pointer(Detail::ProcessedCommandLine, Parameter.Identifier, &LProgramArgument::Identifier)};
    if (!Argument)
    {
        for (LString const& Variation : Parameter.Variations)
        {
            Argument = algo::find_pointer(Detail::ProcessedCommandLine, Variation, &LProgramArgument::Identifier);
            if (Argument)
            {
                break;
            }
        }
    }
    if (Argument)
    {
        if ((Parameter.Flags & EProgramParameterBits::StoreTrue) && Argument->IsStoreTrue())
        {
            return Argument;
        }
        if ((Parameter.Flags & EProgramParameterBits::Value) && Argument->IsValue())
        {
            return Argument;
        }
        if ((Parameter.Flags & EProgramParameterBits::List) && Argument->IsList())
        {
            return Argument;
        }
        LOG_FATAL(LogProgramArguments, "Program argument [{}] does not match the expected type for parameter [{}]."
            , Argument->Identifier, Parameter.Identifier)
    }
    return nullptr;
}

inline void PrettyPrintVersion() noexcept
{
    LOG_INFO(LogCli, "Engine version [{}] @mzoesch at [{} - {}] on {} in {}.",
        Jafg::BuildInfo::GetEngineVersionStr(),
        Jafg::BuildInfo::GetBuildTime(), Jafg::BuildInfo::GetBuildDate(),
        Jafg::BuildInfo::GetVcsBranch(), Jafg::BuildInfo::GetVcsRevision()
        )
}
inline void PrettyPrintApiUsage() noexcept
{
    u64 MaxSize{};
    algo::for_each(Detail::RegisteredProgramParameters, [&MaxSize](LProgramParameter* Param)
    {
        MaxSize = maths::max(MaxSize, static_cast<u64>(Param->Identifier.size()));
    });
    LOG_INFO(LogCli, "Available command line parameters:")
    algo::for_each(Detail::RegisteredProgramParameters, [MaxSize](LProgramParameter* Param)
    {
        LOG_INFO(LogCli, "  -{:<{}} : {}", Param->Identifier, MaxSize, Param->Description)
        LOG_INFO(LogCli, "   {:<{}}   Flags: {}", "", MaxSize, LexToString(Param->Flags))
    });
    return;
}

inline std::chrono::high_resolution_clock::time_point GetStaticStorageInitializationTime() noexcept
{
    return Detail::StaticContainerInitializationTime;
}

inline f64 GetElapsedTime() noexcept
{
    return algo::time_diff(GetStaticStorageInitializationTime(), std::chrono::high_resolution_clock::now());
}

} /* ~Namespace Application */

namespace Hal
{

//#
//# Very dangerous function. Use with care and never in critical code paths.
//# Currently not supported for all platforms.
//#
ENGINE_API void Sleep(f64 InSeconds);

} /* ~Namespace PlatformHal */

} /* ~Namespace Jafg */
