// Copyright mzoesch. All rights reserved.

#include "Core/App.h"
#include "Core/Uuid.h"
#include "Stats/Stats.h"
#include "Runtime/Parameter.h"
#include "Async/TaskUtility.h"
#include <unistd.h>

namespace finder::detail
{

path _engine_root_dir_slow()
{
    check(Jafg::Tasks::IsOnMasterThread())
    LPath RealRootDir{self_proc_dir_slow()};
    while (!RealRootDir.empty())
    {
        if (exists(RealRootDir/"jafg.jafgworkspace"))
        {
            break;
        }
        if (RealRootDir.has_parent_path())
        {
            RealRootDir.assign(RealRootDir.parent_path());
        }
        else
        {
            break;
        }
    }

    if (!is_regular_file(RealRootDir/"jafg.jafgworkspace"))
    {
        LOG_FATAL(LogSystem, "[{}]: Engine evaluated its root directly wrongfully", RealRootDir)
    }

    return RealRootDir;
}

path self_proc_slow()
{
    check(Jafg::Tasks::IsOnMasterThread())

static LPath CachedSelfProcDir;
    if (CachedSelfProcDir.empty())
    {
        check(Jafg::Tasks::IsOnMasterThread())
#if JAFG_PLATFORM_LINUX
#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wunsafe-buffer-usage"
#endif /* JAFG_WITH_CLANG */
        char Buffer[JAFG_PLATFORM_MAX_PATH] = { 0 };
        auto Ret{ readlink("/proc/self/exe", Buffer, JAFG_PLATFORM_MAX_PATH) };
        if (Ret == -1)
        {
            panic("Failed to read the symbolic link.")
        }
        Buffer[Ret] = '\0';
#if JAFG_WITH_CLANG
#pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */
        CachedSelfProcDir = LString{Buffer};
#elif JAFG_PLATFORM_WINDOWS
        TCHAR Buffer[JAFG_PLATFORM_MAX_PATH]{ 0 };
        GetModuleFileName(nullptr, Buffer, JAFG_PLATFORM_MAX_PATH);
        CachedSelfProcDir = LPath{Buffer};
        CachedSelfProcDir = CachedSelfProcDir.lexically_normal();
#else /* JAFG_PLATFORM_WINDOWS */
#error "Missing implementation for this platform."
#endif /* !JAFG_PLATFORM_WINDOWS */
    }

    return CachedSelfProcDir;
}

ENGINE_API path dump_file{ "unsettling.dump"};

#if JAFG_PLATFORM_LINUX
    ENGINE_API std::optional<path> _lnx_gdb;
#endif /* JAFG_PLATFORM_LINUX */

} /* ~Namespace finder::detail */

namespace Jafg::App
{

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdate-time"
#endif /* JAFG_WITH_CLANG */
LString const& BuildTime() noexcept
{
    static LString BuildTime{__TIME__};
    return BuildTime;
}

LString const& BuildDate() noexcept
{
    static LString BuildDate{__DATE__};
    return BuildDate;
}
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

LString const& BuildVcsBranch() noexcept
{
    static LString Branch{DETAIL_ENGINE_VCS_BRANCH};
    return Branch;
}

LString const& BuildVcsRevision() noexcept
{
    static LString Branch{DETAIL_ENGINE_VCS_REVISION};
    return Branch;
}

LString const& VcsRemote() noexcept
{
    static LString Remote{DETAIL_ENGINE_VCS_REMOTE};
    return Remote;
}

LString const& VcsUpstream() noexcept
{
    static LString Upstream{DETAIL_ENGINE_VCS_UPSTREAM};
    return Upstream;
}

LString const& Homepage() noexcept
{
    static LString Homepage{DETAIL_ENGINE_HOMEPAGE_URL};
    return Homepage;
}

LEngineVersion EngineVersion() noexcept
{
    return
    {
        .Major = DETAIL_ENGINE_VERSION_MAJOR,
        .Minor = DETAIL_ENGINE_VERSION_MINOR,
        .Patch = DETAIL_ENGINE_VERSION_PATCH,
        .Revision = DETAIL_ENGINE_VERSION_REVISION,
    };
}

LString const& EngineVersionHash() noexcept
{
    static LString Hash{DETAIL_ENGINE_VERSION_HASH};
    return Hash;
}

LString const& CompilerVersion() noexcept
{
#if JAFG_WITH_CLANG
    static LString Version{__VERSION__};
#elif JAFG_WITH_GCC
    static LString Version{__VERSION__};
#elif JAFG_WITH_MSVC
    static LString Version{std::to_string(_MSC_FULL_VER)};
#else /* JAFG_WITH_MSVC */
    #error "Missing compiler implementation."
#endif /* !JAFG_WITH_MSVC */

    return Version;
}

LString const& CxxStandard() noexcept
{
    static LString Standard{algo::sprintf("C++{}", __cplusplus)};
    return Standard;
}

LStringView GetEngineRootRelativeBinaryPath() noexcept { return LStringView{DETAIL_ENGINE_ROOT_BINARY_PATH}; }
LStringView GetTargetPlatform() noexcept { return LStringView{DETAIL_ENGINE_TARGET_PLATFORM}; }
LStringView GetTargetArchitecture() noexcept { return LStringView{DETAIL_ENGINE_TARGET_ARCHITECTURE}; }
LStringView GetTargetType() noexcept { return LStringView{DETAIL_ENGINE_TARGET_TYPE}; }
LStringView GetTargetConfiguration() noexcept { return LStringView{DETAIL_ENGINE_TARGET_CONFIGURATION}; }

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
ENGINE_API LProgramParameter DumpStack{{
    .Identifier = "Jafg.DumpStack",
    .Description = "Whether to also dump the stack instead of only a core dump if supported.",
    }};
ENGINE_API LProgramParameter AllowProfiling{{
    .Identifier = "Jafg.AllowProfiling",
    .Description = "Whether to allow profiling and stats gathering.",
    }};
ENGINE_API LProgramParameter PauseBeforeExit{{
    .Identifier = "Jafg.PauseBeforeExit",
    .Description = "Whether to pause before exiting the application.",
    }};

} /* ~Namespace Jafg::App */

namespace Jafg::App::Detail
{

ENGINE_API bool bAlreadyCrashed{};
ENGINE_API bool bSuppressCrashDialog{};
ENGINE_API bool bDumpStack{};

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

ENGINE_API algo::clock::time_point StaticContainerInitializationTime{ algo::now() };

} /* ~Namespace Jafg::App::Detail */

namespace Jafg::App::Detail
{

ENGINE_API TArray<LProgramParameter*> RegisteredProgramParameters;

} /* ~Namespace Jafg::App::Detail */

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
    if (App::WillShortlyTerminate() == false)
    {
        algo::erase(&App::Detail::RegisteredProgramParameters, this);
    }
}

void Jafg::App::Detail::WaitForDebuggerGracefully(bool bAllowInstantBreak)
{
    LOG_INFO(LogJafgInternal, "Waiting for debugger ...")
    Jafg::FlushOutStreams();

    while (!App::IsTracerPidValidNow())
    {
        App::SleepNoStats(1.0);
    }

    LOG_INFO(LogJafgInternal, "Debugger attached - continuing.")
    Jafg::FlushOutStreams();

    if (bAllowInstantBreak)
    {
        if (!algo::contains(App::GetRawCommandLine(), std::string_view{"-Jafg.IgnoreInstantDebuggerBreak"}))
        {
            JAFG_PLATFORM_BREAK()
        }
    }

    return;
}

void Jafg::App::Detail::BeginExitIfRequested() noexcept
{
    check(Tasks::IsOnMasterThread())
    if (bShouldRequestExit)
    {
       bEngineRequestingExit = true;
    }
    return;
}

void Jafg::App::RequestEngineExit() noexcept
{
    std::scoped_lock Lock{Detail::ExitMutex};
    if (!App::WillShortlyTerminate())
    {
        Detail::bShouldRequestExit = true;
    }
}

void Jafg::App::RequestEngineExit(LString Reason) noexcept
{
    std::scoped_lock Lock{Detail::ExitMutex};
    if (!App::WillShortlyTerminate())
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitReason = std::move(Reason);
    }
}

void Jafg::App::RequestEngineExit(i32 CustomExitStatus) noexcept
{
    std::scoped_lock Lock{Detail::ExitMutex};
    if (!App::WillShortlyTerminate())
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitStatusOverride = CustomExitStatus;
    }
}

void Jafg::App::RequestEngineExit(i32 CustomExitStatus, LString Reason) noexcept
{
    std::scoped_lock Lock{Detail::ExitMutex};
    if (WillShortlyTerminate() == false)
    {
        Detail::bShouldRequestExit = true;
        Detail::CustomExitStatusOverride = CustomExitStatus;
        Detail::CustomExitReason = std::move(Reason);
    }
}

void Jafg::App::Sleep(f64 InSeconds)
{
    STAT_CYCLE_FUNCTION()
    SleepNoStats(InSeconds);
}
