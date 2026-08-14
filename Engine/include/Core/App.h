// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Arguments.h"
#include "Core/Parameter.h"
#include "Core/Argument.h"
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
//# Default remote of the vcs.
NODISCARD NOINLINE ENGINE_API LString const& VcsRemote() noexcept;
//# Engine upstream.
NODISCARD NOINLINE ENGINE_API LString const& VcsUpstream() noexcept;
//# Engine homepage.
NODISCARD NOINLINE ENGINE_API LString const& Homepage() noexcept;

//# Engine version.
NODISCARD ENGINE_API LEngineVersion EngineVersion() noexcept;
//# Vcs hash from the version -- not from the current engine build.
NODISCARD ENGINE_API LString const& EngineVersionHash() noexcept;

//# Compiler version used for the engine build.
NODISCARD NOINLINE ENGINE_API LString const& CompilerVersion() noexcept;
//# Cxx standard for the engine.
NODISCARD NOINLINE ENGINE_API LString const& CxxStandard() noexcept;

//# Root binary path where this engine build searches for its resources.
ENGINE_API LStringView GetEngineRootRelativeBinaryPath() noexcept;
//# E.g.: "wasm", "lnx", ...
ENGINE_API LStringView GetTargetPlatform() noexcept;
//# E.g.: "x86_64", "x86", ...
ENGINE_API LStringView GetTargetArchitecture() noexcept;
//# E.g.: "cl", "daemon", ...
ENGINE_API LStringView GetTargetType() noexcept;
//# E.g.: "dbg", "ship", ...
ENGINE_API LStringView GetTargetConfiguration() noexcept;

ENGINE_API extern LProgramParameter CoreHelp;
ENGINE_API extern LProgramParameter Version;
ENGINE_API extern LProgramParameter Help;
ENGINE_API extern LProgramParameter Daemon;
ENGINE_API extern LProgramParameter Headless;
ENGINE_API extern LProgramParameter Quiet;
ENGINE_API extern LProgramParameter SkipForeignInit;
ENGINE_API extern LProgramParameter SkipLoop;
ENGINE_API extern LProgramParameter DisallowAnsi;
ENGINE_API extern LProgramParameter EmitInformation;
ENGINE_API extern LProgramParameter WaitForDebugger;
ENGINE_API extern LProgramParameter IgnoreInstantDebuggerBreak;
ENGINE_API extern LProgramParameter AlwaysReportCrash;
ENGINE_API extern LProgramParameter DumpStack;
ENGINE_API extern LProgramParameter AllowProfiling;
ENGINE_API extern LProgramParameter PauseBeforeExit;
ENGINE_API extern LProgramParameter SkipTrivialTests;

namespace Detail
{

ENGINE_API extern bool bAlreadyCrashed;
//# Not supported in all configurations.
ENGINE_API extern bool bSuppressCrashDialog;
//# Not supported in all configurations.
ENGINE_API extern bool bDumpStack;
ENGINE_API bool HasArgumentToWaitForDebuggerVeryEarlyOnly();
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

ENGINE_API extern bool DisallowAnsi;

#if WITH_STATS
    ENGINE_API extern bool AllowProfiling;
#endif /* WITH_STATS */

//#
//# The command line. A parameter is defined as the following:
//#   --parameter                                                       StoreTrue
//#   --parameter=value                  (no spaces allowed)            Value
//#   --parameter="string value"         (escape " with \)              Value
//#   --parameter item item item         (no -/-- allowed on items)     List
//#
//# A parameter must accept at least one of these above categories (StoreTrue,Value,List).
//#
//# Or some parameters have short identifiers (for StoreTrue only) that can be chained.
//# E.g. if there a two parameters that also allow for a and b. Then the following are also valid -- in addition
//# to the full ones mentioned above:
//#     -a -b
//#     -ab
//#     -ba
//#     For any number of short identifiers parameters, any combination is valid.
//#
ENGINE_API extern TArray<LString> RawCommandLine;
ENGINE_API extern TArray<LProgramArgument> ProcessedCommandLine;

//# Init time of Jafg. The engine will always use this as the most earliest relative time point.
ENGINE_API extern algo::clock::time_point StaticContainerInitializationTime;

enum struct EAppLockResult
{
    Unique,
    Shared,
    Unknown,
};
NODISCARD ENGINE_API EAppLockResult TryAcquireAppLock();

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
NODISCARD ENGINE_API TArray<LProgramArgument> ReprocessCommandLine(TArray<LString> const& CommandLine) noexcept;

//#
//# Whether the command line contains the given argument.
//# @note This function will not work during static storage initialization or early program startup.
//#
ENGINE_API LProgramArgument const* GetCommandLineArgument(LStringView Parameter) noexcept;
ENGINE_API LProgramArgument const* GetCommandLineArgument(LProgramParameter const& Parameter) noexcept;

ENGINE_API void PrettyPrintVersion() noexcept;
ENGINE_API void PrettyPrintApiUsage() noexcept;

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
