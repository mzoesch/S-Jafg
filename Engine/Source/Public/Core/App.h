// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Arguments.h"
#include "Runtime/Parameter.h"
#include "Runtime/Argument.h"
#include "Stats/StatsForward.h"

namespace Jafg
{

namespace App
{

struct LEngineVersion
{
    u64 Major{};
    u64 Minor{};
    u64 Patch{};
    u64 Revision{};

    NODISCARD FORCEINLINE constexpr std::strong_ordering operator<=>(LEngineVersion const& Other) const = default;
    FORCEINLINE LString ToString() const
    {
        return algo::sprintf("{}.{}.{}.{}", this->Major, this->Minor, this->Patch, this->Revision);
    }
};

//# Engine build time.
NODISCARD NOINLINE ENGINE_API LString const& BuildTime() noexcept;
//# Engine build date.
NODISCARD NOINLINE ENGINE_API LString const& BuildDate() noexcept;
//# Engine branch.
NODISCARD NOINLINE ENGINE_API LString const& BuildVcsBranch() noexcept;
//# Latest engine revision.
NODISCARD NOINLINE ENGINE_API LString const& BuildVcsRevision() noexcept;

//# Engine version.
NODISCARD ENGINE_API LEngineVersion EngineVersion() noexcept;
//# Vcs hash from the version -- not from the current engine build.
NODISCARD ENGINE_API LString const& EngineVersionHash() noexcept;

//# Compiler version used for the engine build.
NODISCARD NOINLINE ENGINE_API LString const& CompilerVersion() noexcept;
//# Cxx standard for the engine.
NODISCARD NOINLINE ENGINE_API LString const& CxxStandard() noexcept;

//# E.g.: "Windows", "Linux", ...
ENGINE_API LStringView GetTargetPlatform() noexcept;
//# E.g.: "x86_64", "x86", ...
ENGINE_API LStringView GetTargetArchitecture() noexcept;
//# E.g.: "Client", "Daemon", ...
ENGINE_API LStringView GetTargetType() noexcept;
//# E.g.: "Debug", "Shipping", ...
ENGINE_API LStringView GetTargetConfiguration() noexcept;
//# E.g.: "Client-Shipping", ...
ENGINE_API LStringView GetTargetCompound() noexcept;
//# E.g.: "Windows-x86_64", "Linux-x86_64", ...
ENGINE_API LStringView GetTargetPlatformCompound() noexcept;
//# E.g.: "Linux-x86_64/Client-Shipping", ...
ENGINE_API LStringView GetTargetPath() noexcept;
//# E.g: "Runtime"
ENGINE_API LStringView GetExpectedRuntime() noexcept;
//# E.g.: "Binaries/Linux-x86_64/Client-Shipping/Runtime", ...
ENGINE_API LStringView GetExpectedRuntimePath() noexcept;

ENGINE_API extern LProgramParameter CoreHelp;
ENGINE_API extern LProgramParameter Version;
ENGINE_API extern LProgramParameter Help;
ENGINE_API extern LProgramParameter WaitForDebugger;
ENGINE_API extern LProgramParameter IgnoreInstantDebuggerBreak;
ENGINE_API extern LProgramParameter AlwaysReportCrash;
ENGINE_API extern LProgramParameter DumpStack;
ENGINE_API extern LProgramParameter AllowProfiling;
ENGINE_API extern LProgramParameter PauseBeforeExit;

namespace Detail
{

ENGINE_API extern bool bAlreadyCrashed;
//# Not supported in all configurations.
ENGINE_API extern bool bSuppressCrashDialog;
//# Not supported in all configurations.
ENGINE_API extern bool bDumpStack;
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

//# Init time of Jafg. The engine will always use this as the most earliest relative time point.
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
    Detail::IsTracerPidValid = Detail::IsTracerPidValidVerySlow();
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
        EngineVersion().ToString(),
        BuildTime(), BuildDate(), BuildVcsBranch(), BuildVcsRevision()
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

inline algo::clock::time_point GetStaticStorageInitializationTime() noexcept
{
    return Detail::StaticContainerInitializationTime;
}

inline f64 GetElapsedTime(algo::clock::time_point Since = algo::now()) noexcept
{
    return algo::time_diff(GetStaticStorageInitializationTime(), Since);
}

//#
//# Very dangerous function. Use with care and never in critical code paths.
//# Currently not supported for all platforms.
//#
ENGINE_API void Sleep(f64 InSeconds);
ENGINE_API void SleepNoStats(f64 InSeconds);

} /* ~Namespace App */

} /* ~Namespace Jafg */

namespace Finder
{

inline LPath GetRootBinaryDirectory() noexcept { auto Out{LPath{"Binaries"}/Jafg::App::GetTargetPath()}; Out.make_preferred(); return Out; }

} /* ~Namespace Finder */
