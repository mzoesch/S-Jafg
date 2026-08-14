// Copyright mzoesch. All rights reserved.

#include "Core/App.h"
#include "Core/Uuid.h"
#include "Stats/Stats.h"
#include "Core/Parameter.h"
#include "Core/TaskUtility.h"
#if JAFG_PLATFORM_LINUX
    #include <unistd.h>
    #include <sys/file.h>
    #include <fcntl.h>
#endif /* JAFG_PLATFORM_LINUX */

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

ENGINE_API path dump_file{ "unsettling.dump" };

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
    .ShortIdentifier = "h",
    .Variations = {"help"},
    }};
ENGINE_API LProgramParameter Version{{
    .Identifier = "Version",
    .Description = "Shows the version of the engine.",
    .ShortIdentifier = "v",
    .Variations = {"version"},
    }};
ENGINE_API LProgramParameter Help{{
    .Identifier = "Jafg.VerboseHelp",
    .Description = "Shows help window for all default loaded plugins.",
    .ShortIdentifier = "H",
    }};
ENGINE_API LProgramParameter Daemon{{
    .Identifier = "Jafg.Daemon",
    .Description = "Run jafg as a daemon.",
    .ShortIdentifier = "d",
    }};
ENGINE_API LProgramParameter Headless{{
    .Identifier = "Jafg.Headless",
    .Description = "Run jafg headlessly; Skip default surface. Frontend will not be initialized and set to headless.",
    .ShortIdentifier = "s",
    }};
ENGINE_API LProgramParameter Quiet{{
    .Identifier = "Jafg.Quiet",
    .Description = "Makes jafg shut up by not emitting to stdout. Might not be respected by everyone.",
    .ShortIdentifier = "q",
    }};
ENGINE_API LProgramParameter SkipForeignInit{{
    .Identifier = "Jafg.SkipForeignInit",
    .Description = "Only load jafg and its core. Skip all plugins. In units builds this of course includes everything inside said unity.",
    .ShortIdentifier = "f",
    }};
ENGINE_API LProgramParameter SkipLoop{{
    .Identifier = "Jafg.SkipLoop",
    .Description = "Jafg fully initializes everything. But immediately tears down before executing the main loop.",
    .ShortIdentifier = "l",
    }};
ENGINE_API LProgramParameter DisallowAnsi{{
    .Identifier = "Jafg.DisallowAnsi",
    .Description = "Jafg will not print ansi characters to stdout or stderr (but vendors still might).",
    .ShortIdentifier = "a",
    }};
ENGINE_API LProgramParameter EmitInformation{{
    .Identifier = "Jafg.EmitInformation",
    .Description = "Allows jafg to always emit messages from the LogInformation category even if Jafg.Quiet is set.",
    .ShortIdentifier = "i",
    }};
ENGINE_API LProgramParameter WaitForDebugger{{
    .Identifier = "Jafg.WaitForDebugger",
    .Description = "Whether to wait for a debugger to attach to this process before continuing execution.",
    .ShortIdentifier = "D",
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
    .ShortIdentifier = "p",
    }};
ENGINE_API LProgramParameter PauseBeforeExit{{
    .Identifier = "Jafg.PauseBeforeExit",
    .Description = "Whether to pause before exiting the application.",
    }};
ENGINE_API LProgramParameter SkipTrivialTests{{
    .Identifier = "Jafg.SkipTrivialTests",
    .Description = "Whether to skip trivial tests that are run at startup in development builds.",
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

ENGINE_API bool DisallowAnsi{};

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

ENGINE_API std::size_t GLogMessagesLimit{ 2048 };
ENGINE_API std::list<LLogMessage> GUnprocessedLogMessages;
ENGINE_API std::list<LLogMessage> GLogMessages;
ENGINE_API algo::tas_lock GLogMessagesLock;

ENGINE_API bool GIsQuiet{};
ENGINE_API bool GAllowHelp{};
ENGINE_API bool GAllowInformation{};

void EmitLogsToStdout() noexcept
{
    STAT_CYCLE_FUNCTION()

    std::scoped_lock Lock{GLogMessagesLock};
    auto It{GUnprocessedLogMessages.begin()};
    while (It != GUnprocessedLogMessages.end())
    {
        if (   GIsQuiet
            && !(GAllowHelp && (It->Category == "LogHelp"sv))
            && !(GAllowInformation && (It->Category == "LogInformation"sv))
        )
        {
            GUnprocessedLogMessages.pop_front();
            It = GUnprocessedLogMessages.begin();
            continue;
        }

#if JAFG_PLATFORM_WASM
        if (It->Verbosity == ELogVerbosity::Warning)
        {
            ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_WARN, It->Message);
        }
        else if (It->Verbosity == ELogVerbosity::Error || It->Verbosity == ELogVerbosity::Fatal)
        {
            ::emscripten_log(EM_LOG_CONSOLE | EM_LOG_ERROR, It->Message);
        }
        else
        {
            ::emscripten_log(EM_LOG_CONSOLE, It->Message);
        }
#else /* JAFG_PLATFORM_WASM */
        if (App::Detail::DisallowAnsi)
        {
            std::cout << It->Message << '\n';
        }
        else
        {
            std::cout << GetColorForVerbosity(It->Verbosity) << It->Message << JAFG_LOG_COLOR_END << '\n';
        }
#endif /* !JAFG_PLATFORM_WASM */

        GLogMessages.emplace_back(std::move(*It));
        GUnprocessedLogMessages.pop_front();
        It = GUnprocessedLogMessages.begin();
    }

    while (GLogMessages.size() > GLogMessagesLimit)
    {
        GLogMessages.pop_front();
    }
}

void TryFlushStdout() noexcept
{
#if JAFG_PLATFORM_SUPPORTS_STD_FLUSH
    std::cout.flush();
    std::cerr.flush();
#endif /* !JAFG_PLATFORM_SUPPORTS_STD_FLUSH */
}

void EmitAndFlushLogs() noexcept
{
    EmitLogsToStdout();
    TryFlushStdout();
}

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

bool Jafg::App::Detail::HasArgumentToWaitForDebuggerVeryEarlyOnly()
{
    for (auto& Arg: App::GetRawCommandLine())
    {
        if (Arg == App::WaitForDebugger.Identifier)
        {
            return true;
        }
        check(App::WaitForDebugger.Variations.empty())
        if (Arg.starts_with("-") && !Arg.starts_with("--"))
        {
            check(App::WaitForDebugger.ShortIdentifier.has_value())
            if (Arg.contains(*App::WaitForDebugger.ShortIdentifier))
            {
                return true;
            }
        }
    }
    return false;
}

void Jafg::App::Detail::WaitForDebuggerGracefully(bool bAllowInstantBreak)
{
    LOG_INFO(LogJafgInternal, "Waiting for debugger ...")
    Jafg::Detail::EmitAndFlushLogs();

    while (!App::IsTracerPidValidNow())
    {
        App::SleepNoStats(1.0);
    }

    LOG_INFO(LogJafgInternal, "Debugger attached - continuing.")
    Jafg::Detail::EmitAndFlushLogs();

    if (bAllowInstantBreak)
    {
        if (!algo::contains(App::GetRawCommandLine(), std::string_view{"--Jafg.IgnoreInstantDebuggerBreak"}))
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

Jafg::App::Detail::EAppLockResult Jafg::App::Detail::TryAcquireAppLock()
{
#if JAFG_PLATFORM_LINUX
    auto Fd{::open(LITERAL_TEXT("Temp/.Jafg.AppLock"), O_CREAT|O_RDWR, 0666)};
    if (Fd == -1)
    {
        return EAppLockResult::Unknown;
    }
    if (::flock(Fd, LOCK_EX|LOCK_NB) == -1)
    {
        ::close(Fd);
        return EAppLockResult::Shared;
    }
    /* Intentionally leaking Fd. */
    return EAppLockResult::Unique;
#elif JAFG_PLATFORM_WINDOWS
    HANDLE Mutex{::CreateMutexW(nullptr, TRUE, LITERAL_TEXT("Jafg::AppLock"))};
    if (!Mutex)
    {
        return EAppLockResult::Unknown;
    }
    if (::GetLastError() == ERROR_ALREADY_EXISTS)
    {
        ::CloseHandle(Mutex);
        return EAppLockResult::Shared;
    }
    /* Intentionally leaking Mutex. */
    return EAppLockResult::Unique;
#else /* JAFG_PLATFORM_WINDOWS */
    #error "Missing implementation for platform."
#endif /* !JAFG_PLATFORM_WINDOWS */
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

TArray<Jafg::LProgramArgument> Jafg::App::ReprocessCommandLine(TArray<LString> const& CommandLine) noexcept
{
    TArray<LProgramArgument> Result;

    LProgramArgument* CurrentList{};
    algo::for_each(CommandLine, [&](LString const& Parameter)
    {
        if (Parameter.starts_with("--"))
        {
            CurrentList = nullptr;
            if (auto Idx{Parameter.find('=')}; Idx != LString::npos)
            {
                Result.emplace_back(algo::sub(Parameter, 2, Idx), algo::right_chop(Parameter, Idx + 1));
            }
            else
            {
                Result.emplace_back(algo::right_chop(Parameter, 2));
                CurrentList = &Result.back();
            }
        }
        else if (Parameter.starts_with('-'))
        {
            CurrentList = nullptr;
            auto Regex{"^-[a-zA-Z]+$"s};
            if (std::regex_match(Parameter, std::regex{Regex}))
            {
                for (auto const& C: algo::right_chop(Parameter, 1))
                {
                    Result.push_back({.Identifier={C}});
                }
            }
            else
            {
                LOG_WARNING(LogLaunch, "Invalid Syntax for [{}]; does not match [{}].", Parameter, Regex)
                Result.emplace_back(Parameter);
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

    return Result;
}

Jafg::LProgramArgument const* Jafg::App::GetCommandLineArgument(LStringView Parameter) noexcept
{
    return algo::find_pointer(Detail::ProcessedCommandLine, Parameter, &LProgramArgument::Identifier);
}

Jafg::LProgramArgument const* Jafg::App::GetCommandLineArgument(LProgramParameter const& Parameter) noexcept
{
    LProgramArgument const* Argument{algo::find_pointer(Detail::ProcessedCommandLine, Parameter.Identifier, &LProgramArgument::Identifier)};
    if (!Argument)
    {
        if (Parameter.ShortIdentifier)
        {
            /*
             * A parameter that accepts short identifier must have the StoreTrue flag set, as with short
             * identifiers passing any kind of additional value is not possible.
             */
            check(!!(Parameter.Flags & EProgramParameterBits::StoreTrue))
            Argument = algo::find_pointer(Detail::ProcessedCommandLine, *Parameter.ShortIdentifier, &LProgramArgument::Identifier);
        }
    }
    if (!Argument)
    {
        for (LString const& Variation: Parameter.Variations)
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

void Jafg::App::PrettyPrintVersion() noexcept
{
    LOG_INFO(LogHelp, "Engine version [{}] @mzoesch at [{} - {}] on {} in {}.",
        EngineVersion().ToString(),
        BuildTime(), BuildDate(), BuildVcsBranch(), BuildVcsRevision()
        )
}

void Jafg::App::PrettyPrintApiUsage() noexcept
{
    u64 MaxSize{};
    algo::for_each(Detail::RegisteredProgramParameters, [&MaxSize](LProgramParameter* Param)
    {
        MaxSize = maths::max(MaxSize, static_cast<u64>(Param->Identifier.size()));
    });
    LOG_INFO(LogHelp, "Available command line parameters:")
    for (auto& Param: Detail::RegisteredProgramParameters)
    {
        std::stringstream SS;
        if (Param->ShortIdentifier)
        {
            SS << "-" << *Param->ShortIdentifier;
        }
        bool bFirst{ true };
        for (auto& Variations: Param->Variations)
        {
            if (bFirst && !Param->ShortIdentifier)
            {
                SS << "--" << Variations;
            }
            else
            {
                SS << ", --" << Variations;
            }
            bFirst = false;
        }

        LOG_INFO(LogHelp, "  -{:<{}} : {}", Param->Identifier, MaxSize, Param->Description)
        if (auto Str{SS.str()}; !Str.empty())
        {
            LOG_INFO(LogHelp, "   {:<{}}   Variations: {}", "", MaxSize, Str)
        }
        LOG_INFO(LogHelp, "   {:<{}}   Flags: {}", "", MaxSize, LexToString(Param->Flags))
    }
}

void Jafg::App::Sleep(f64 InSeconds)
{
    STAT_CYCLE_FUNCTION()
    SleepNoStats(InSeconds);
}
