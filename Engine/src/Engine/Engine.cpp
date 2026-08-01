// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Framework/TextureSubsystem.h"
#include "Framework/MeshSubsystem.h"
#include "Core/TaskUtility.h"
#include "Framework/TickedRunnable.h"
#include "Cli/CliExtended.h"
#include "Cli/CliPrimitives.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Framework/EngineSubsystem.h"
#include "Cli/CommandLineInterface.h"
#include "Stats/Stats.h"
#include "EngineRunnable.h"
#include "Cli/ReSTCliPreferences.h"
#include "User/UserPreferences.h"

#ifndef JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES
    #if JAFG_IN_SHIPPING
        #define JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES                      2.0
    #else /* JAFG_IN_SHIPPING */
        #define JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES                      0.7
    #endif /* !JAFG_IN_SHIPPING */
#endif /* JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES */

ENGINE_API Jafg::LEngine const* GEngine{};
namespace Jafg::Detail
{
ENGINE_API LEngine* GMutableEngine{};
} /* ~Namespace Jafg::Detail */

bool Jafg::LWorldStorage::IsValid() const noexcept
{
    check(GEngine && "Absence of GEngine if undefined behavior.")
    if (this->World == nullptr)
    {
        return false;
    }
    return GEngine->IsWorldValid(this->World);
}

Jafg::LEngine::LEngine()
{
    STAT_CYCLE_FUNCTION()
    if (ETaskExit::Type const Rc{Tasks::LaunchNamedThread<LEngineRunnable>(ENamedThreads::WorkerThread, "WorkerThread")};
        Rc != ETaskExit::Success
        )
    {
        LOG_FATAL(LogLaunch, "Failed to create worker thread: [{}].", static_cast<i32>(Rc))
    }
}

void Jafg::LEngine::Initialize()
{
    STAT_CYCLE_FUNCTION()

    Private::AddPrimitivesToCli(&this->GetCommandLineInterface());
    Private::AddExtendedPrimitivesToCli(&this->GetCommandLineInterface());

    /* Engine stuff. */
    {
        const bool bValid_TypeWorld{this->CommandLineInterface.RegisterType({"World", "A world registered to the engine.", "",
        [](LCommandArgs const& Args, auto* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.empty())
            {
                return false;
            }

            if (GEngine == nullptr)
            {
                return false;
            }

            const bool bValid{algo::contains(
                GEngine->GetTracks(), Args[*Cursor].Name
                , [](auto const& E){ return E.ChildWorld->GetHumanReadableName(); }
                )};
            if (bValid)
            {
                ++*Cursor;
            }

            return bValid;
        },
        nullptr,
        [](LCommandArgs const& Args, const auto Cursor, const u32 MaxSuggestions) -> TArray<LString>
        {
            const LCommandArgs* Target { nullptr };

            if (algo::valid_index(Args.SubArgs, Cursor))
            {
                Target = &Args[Cursor];
            }

            if (GEngine == nullptr)
            {
                return { };
            }

            TArray<LString> Out;

            for (auto const& Tracks : GEngine->GetTracks())
            {
                if (Out.size() >= MaxSuggestions)
                {
                    break;
                }

                check( Tracks.ChildWorld )

                if (Target)
                {
                    if (Tracks.ChildWorld->GetHumanReadableName().starts_with(Target->Name))
                    {
                        Out.emplace_back(Tracks.ChildWorld->GetHumanReadableName());
                    }
                }
                else
                {
                    Out.emplace_back(Tracks.ChildWorld->GetHumanReadableName());
                }

                continue;
            }

            return Out;
        }}).IsValid()};
        ensureDiscard(bValid_TypeWorld);

        this->CommandLineInterface.RegisterCommandChecked({"Set", "Set any variable.",
        LCommandParams{}
        .Token(LCliType::Type<LCliVariable>())
        .Token(LCliType{"VarType", "The value to set.",
            {},
            [](LCommandArgs const& Args, auto* Cursor) -> bool
            {
                check(*Cursor < Args.GetArgCount())
                if (Args[*Cursor].Name.empty())
                {
                    return false;
                }
                ++*Cursor;
                return true;
            },
            nullptr,
            [](LCommandArgs const& Args, const auto Cursor, const u32 MaxSuggestions) -> TArray<LString>
            {
                if (algo::valid_index(Args.SubArgs, Cursor - 1) == false)
                {
                    LOG_WARNING(LogCli, "Encountered invalid command args [{}].", Args.GetCatRepresentation())
                    return {};
                }

                if (GEngine == nullptr)
                {
                    return {};
                }

                if (LCliVariable const* TargetVar{GEngine->CommandLineInterface.GetVariable(Args[Cursor - 1].Name)})
                {
                    check(TargetVar->GetType())
                    return TargetVar->GetType()->Suggest(Args, Cursor, MaxSuggestions);
                }

                return {};
            }
        })
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& InArgs, LCommandExecutionResponse& OutResponse)
        {
            check(InArgs.GetArgCount() == 2)
            if (LCliVariable* Var = Detail::GMutableEngine->CommandLineInterface.GetVariable(InArgs[0].Name); Var)
            {
                auto Cursor = 1uz;
                if (Var->GetType()->CanParse(InArgs, &Cursor) == false)
                {
                    OutResponse.Rc = ECommandReturnCode::TypeError;
                    OutResponse.StdOut = algo::sprintf("Cannot parse [{}] as [{}]", InArgs[1].Name, Var->GetType()->GetIdentifier());
                    return;
                }

                if (Var->SetValue(InArgs[1].Name))
                {
                    OutResponse.Rc = ECommandReturnCode::Success;
                    OutResponse.StdOut = algo::sprintf("Updated [{}] to [{}]", Var->GetIdentifier(), Var->GetValue());
                }
                else
                {
                    OutResponse.Rc = ECommandReturnCode::Success;
                }
            }
            else
            {
                OutResponse.Rc = ECommandReturnCode::SemanticError;
                OutResponse.StdOut = algo::sprintf("No such variable [{}]", InArgs[0].Name);
            }
        })});

        this->CommandLineInterface.RegisterCommandChecked({"Get", "Get any variable.",
        LCommandParams{}
        .Token(LCliType::Type("Var", "The variable to get."))
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse) -> void
        {
            check(Args.GetArgCount() == 1)
            if (const LCliVariable* Var = GEngine->CommandLineInterface.GetVariable(Args[0].Name); Var)
            {
                OutResponse.Rc = ECommandReturnCode::Success;
                OutResponse.StdOut = algo::sprintf("[{}] == [{}]", Var->GetIdentifier(), Var->GetValue());
            }
            else
            {
                OutResponse.Rc = ECommandReturnCode::SemanticError;
                OutResponse.StdOut = algo::sprintf("No such variable [{}]", Args[0].Name);
            }
            return;
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"_Break", "Breaks jafg.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 0)
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = "Successfully broken Jafg";
            JAFG_GORGEOUS_BREAK_MSG("Broken through CLI command [_Break].")
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"PrintWorldParams", "Prints the world parameters to the standard output.",
        LCommandParams{}
        .Token(LCliType::Type<LWorld>())
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 1)
            LWorld* World{Args[0].GetAs<LWorld>()};
            OutResponse.StdOut = algo::sprintf("{} params are {}", World->GetHumanReadableName(), World->GetParameters().ToString());
            OutResponse.Rc = ECommandReturnCode::Success;
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"Browse", "Browses to an URL.",
        LCommandParams{}
        .Token(LCliType::Type<LWorld>())
        .Token(LCliType::Type<LString>())
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 2)
            check(Detail::GMutableEngine)
            LWorld& World{*Args[0].GetAs<LWorld>()};
            LString Url{Args[1].GetAs<LString>()};
            Detail::GMutableEngine->Browse(World, Url);
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = algo::sprintf("Browsing to URL [{}] in world [{}]", Url, World.GetHumanReadableName());
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"_Trap", "Traps jafg.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 0)
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = "Successfully trapped Jafg";
            JAFG_GORGEOUS_TRAP_MSG("Trapped through CLI command [_Trap].")
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"_TrapThread", "Traps jafg but not the master thread.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 0)
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = "Send request to trap another thread";
            Tasks::Make(ENamedThreads::WorkerThread, ETaskTime::Whenever, []{JAFG_GORGEOUS_TRAP_MSG("Trapped through CLI command [_Trap].")});
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"_ThrowAccessViolation", "Causes a C access violation.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 0)
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = "Successfully caused a C access violation";
            i64* P{};
            *P = 0xFF;
        })});

        this->GetCommandLineInterface().RegisterCommandChecked({"_ThrowStdAccessViolation", "Causes an access violation in the Stl.",
        LCommandParams{}
        .Exec([](LCommandExecutionInfo const&, LCommandArgs const& Args, LCommandExecutionResponse& OutResponse)
        {
            check(Args.GetArgCount() == 0)
            OutResponse.Rc = ECommandReturnCode::Success;
            OutResponse.StdOut = "Successfully caused a Stl access violation";
            std::vector<u8>{}[1024] = 0xFF;
        })});
    }

    this->Collection.InitializeDeferred(&this->Outer);
    this->Collection.InitializeSubsystems<JEngineSubsystem>();

#if JAFG_WITH_LOCAL_LAYER
    check(!this->LocalEgo.IsDecommissioned())
    this->LocalEgo.Initialize();
#endif /* JAFG_WITH_LOCAL_LAYER */

    return;
}

void Jafg::LEngine::Tick()
{
    STAT_CYCLE_FUNCTION()

    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, 5);

#if JAFG_WITH_LOCAL_LAYER
    this->LocalEgo.Tick(static_cast<f32>(this->DeltaTime));
#endif /* JAFG_WITH_LOCAL_LAYER */

    for (Detail::LWorldTrack& Track : this->Tracks)
    {
        bool bTraveled{};

        if (Track.IsWaitingForTravel())
        {
            if (this->TravelTrack(Track))
            {
                bTraveled = true;
            }
        }

        check(Track.IsValid())
        if (bTraveled == false && Track.ChildWorld->CanTick())
        {
            Track.ChildWorld->Tick(this->DeltaTime);
        }

        continue;
    }

#if JAFG_WITH_LOCAL_LAYER
    for (auto const& Surface : this->LocalEgo.GetFrontend().GetSurfaces())
    {
        Surface->OnRender();
    }
#endif /* JAFG_WITH_LOCAL_LAYER */

    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, 5);

    return;
}

void Jafg::LEngine::TearDown()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogEngine, "Tearing down engine.")

#if JAFG_WITH_REST_CLS
    if (this->GetReSTCli().IsServerRunning())
    {
        this->StopReSTCliServer();
    }
#endif /* JAFG_WITH_REST_CLS */

#if JAFG_WITH_LOCAL_LAYER
    this->LocalEgo.GetFrontend()._Vk_WaitIdle();
#endif /* JAFG_WITH_LOCAL_LAYER */

    LOG_VERBOSE(LogEngine, "Deallocating {} registered tracks.", this->Tracks.size())
    for (auto const& Track : this->Tracks)
    {
        check( Track.IsValid() )
        Track.ChildWorld->TearDown();
        check( Track.ChildWorld->GetWorldState() == EWorldState::WaitingForKill )
        continue;
    }
    algo::orphan(&this->Tracks);

#if JAFG_WITH_LOCAL_LAYER
    check(this->LocalEgo.IsDecommissioned() == false)
    this->LocalEgo.TearDown();
#endif /* JAFG_WITH_LOCAL_LAYER */

    this->Collection.TearDownSubsystems();
    this->Outer.TearDown();

    Tasks::Private::StopAndJoinRemainingThreads();

    LOG_VERBOSE(LogJafgInternal, "Deallocating  {} registered levels.", this->RegisteredLevels.size())
    algo::orphan(&this->RegisteredLevels);

    Detail::GetGlobalCarnifex().KillAllGarbageChildren();

#if JAFG_WITH_FOREIGN_SUPPORT
    if (this->LoadedPlugins.empty() == false)
    {
        LOG_VERBOSE(LogForeign, "There are [{}] loaded plugins. Unloading them now.", this->LoadedPlugins.size())

        while (this->LoadedPlugins.empty() == false)
        {
            LLoadedPlugin& Plugin{this->LoadedPlugins.back()};

            if (Plugin.IsLoaded() == false)
            {
                LOG_WARNING(LogForeign, "Plugin [{}] is not loaded, but the engine still has it registered. Removing it.", Plugin.GetIdentifier() )
                this->LoadedPlugins.pop_back();
                continue;
            }

            if (Plugin.Fetched.bDynUnloadable == false)
            {
                LOG_VERBOSE(LogForeign, "Plugin [{}] is not marked as unloadable. Skipping unload.", Plugin.GetIdentifier() )

                check(Plugin.NativeHandle)
                check(Plugin.Lifetime.get())

                //# This causes a memory leak. But who tf cares.
                this->LoadedPlugins.back().NativeHandle = nullptr;
                this->LoadedPlugins.back().Lifetime.release();

                this->LoadedPlugins.pop_back();
                continue;
            }

            LString CachedIdentifier{Plugin.GetIdentifier()};

            if
            (
                const EPluginLoadReturnCode::Type Rc{ this->UnLoadPlugin(&Plugin, EPluginShutdownReason::EngineTearDown) };
                Rc != EPluginLoadReturnCode::Success
            )
            {
                LOG_WARNING(LogForeign, "Failed to unload plugin [{}] with return code [{}].", CachedIdentifier, LexToString(Rc) )
            }

            continue;
        }
        check(this->LoadedPlugins.empty())
    }
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

    return;
}

void Jafg::LEngine::DefaultTimeAdvance()
{
    STAT_CYCLE_FUNCTION()
    JUserPreferences const& Prefs{GetSingleton<JUserPreferences>()};

    this->LostDeltaTime = 0.0;
    this->IdleDeltaTime = 0.0;

    if (Prefs.MaxTps != JUserPreferences::UnlimitedTps)
    {
#if JAFG_WITH_LOCAL_LAYER
#if JAFG_DO_CHECKS
        static bool bNotified{};
#endif /* JAFG_DO_CHECKS */
        if (auto PresentMode{this->GetLocalEgo().GetFrontend().Vk_GetFirstSurfacePresentMode()};
            PresentMode && !rhi::does_present_mode_allow_uncapped_tps(*PresentMode))
        {
#if JAFG_DO_CHECKS
            if (!bNotified)
            {
                bNotified = true;
                LOG_WARNING(LogEngine, "The current present mode [{}] is a blocking mode. MaxTps of [{}] is ignored."
                    , serde::to_string(*PresentMode), *Prefs.MaxTps)
            }
#endif /* JAFG_DO_CHECKS */
        }
        else
#endif /* JAFG_WITH_LOCAL_LAYER */
        {
#if JAFG_DO_CHECKS
            bNotified = false;
#endif /* JAFG_DO_CHECKS */
            if (auto ElapsedTime{algo::time_diff(App::GetStaticStorageInitializationTime(), algo::now()) - GEngine->FrameStartElapsedTime};
                ElapsedTime < 1.0 / *Prefs.MaxTps)
            {
                auto SleepStart{algo::now()};
                f64 IdleTime{maths::max((1.0 / *Prefs.MaxTps) - ElapsedTime, 0.0)};
                if (constexpr auto Spin{0.001}; IdleTime > Spin)
                {
                    App::SleepNoStats(IdleTime - Spin);
                }
                while (algo::time_diff(SleepStart, algo::now()) < IdleTime)
                {
                    std::this_thread::yield();
                }

                this->IdleDeltaTime = algo::time_diff(SleepStart, algo::now());
                if (this->IdleDeltaTime > this->CurrentStat.HighestIdle)
                {
                    this->CurrentStat.HighestIdle = this->IdleDeltaTime;
                }
            }
        }
    }

    this->FrameStartTimePoint = algo::now();
    this->PreviousFrameStartElapsedTime = std::exchange(this->FrameStartElapsedTime, App::GetElapsedTime(this->FrameStartTimePoint));

    this->DeltaTime = this->FrameStartElapsedTime - this->PreviousFrameStartElapsedTime;
    if (this->DeltaTime < this->CurrentStat.Low)
    {
        this->CurrentStat.Low = this->DeltaTime;
    }
    if (this->DeltaTime > this->CurrentStat.High)
    {
        this->CurrentStat.High = this->DeltaTime;
    }

    this->RealDeltaTime = this->DeltaTime;

    ++this->FrameCount;
    ++this->CurrentStat.FrameCount;

    if (this->DeltaTime > LEngine::MaxDeltaTime)
    {
        if constexpr (IS_COMPILED_LOG(LogLaunch, Warning))
        {
            if (this->DeltaTime > JAFG_LOG_TIME_FOR_VERY_LONG_FRAMES)
            {
                LOG_WARNING(LogLaunch, "Very long frame detected: {} seconds.", this->DeltaTime)
            }
        }
        this->LostDeltaTime = this->DeltaTime - LEngine::MaxDeltaTime;
        if (this->CurrentStat.HighestLoss < this->LostDeltaTime)
        {
            this->CurrentStat.HighestLoss = this->LostDeltaTime;
        }
        this->DeltaTime = LEngine::MaxDeltaTime;
    }

    if (std::chrono::duration<f64>(this->FrameStartTimePoint - this->LastStatisticsTime).count() > this->StatisticsPeriod)
    {
        this->PreviousStat = this->CurrentStat;
        algo::swap_default(&this->CurrentStat);
        this->LastStatisticsTime = this->FrameStartTimePoint;
    }

    return;
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot check for rendering state. */
// ReSharper disable once CppMemberFunctionMayBeStatic
bool Jafg::LEngine::CanEverRender() const noexcept
{
#if JAFG_WITH_LOCAL_LAYER
    return true;
#else /* JAFG_WITH_LOCAL_LAYER */
    return false;
#endif /* !JAFG_WITH_LOCAL_LAYER */
}

void Jafg::LEngine::RegisterClassOuter(LClassOuter* Outer)
{
    check( Tasks::IsOnMasterThread() )
    check( Outer )

    if (algo::contains(this->KnownOuters, Outer))
    {
        panic( "Outer already registered." )
    }

    this->KnownOuters.emplace_back(Outer);
}

void Jafg::LEngine::UnregisterClassOuter(LClassOuter* Outer)
{
    check( Tasks::IsOnMasterThread() )
    check( Outer )

    if (auto It{algo::find(this->KnownOuters, Outer)}; It != this->KnownOuters.end())
    {
        this->KnownOuters.erase(It);
    }
    else
    {
        panicMsgf( "Outer [{}] was not registered to engine.", Outer->GetHumanReadableName() )
    }
}

Jafg::Detail::LWorldTrackInitializer Jafg::LEngine::SummonWorld(Detail::LWorldTrack::CreateInfo Info)
{
    check(Tasks::IsOnMasterThread())
    if constexpr (IS_COMPILED_LOG(LogEngine, Warning))
    if (algo::contains(this->Tracks, Info.HumanReadableName, [](auto const& E){ return E.ChildWorld->GetHumanReadableName(); }))
    {
        LOG_WARNING(LogEngine, "A world with the name [{}] is already summoned.", Info.HumanReadableName)
    }
    return {LWorldStorage{&this->Tracks.emplace_back(std::move(Info)).GetWorld()}};
}

bool Jafg::LEngine::IsWorldValid(LWorld const* World) const noexcept
{
    return World && algo::contains_if(this->Tracks, [World](auto const& E) -> bool
    {
        if (E.ChildWorld.get() == World)
        {
            return E.IsValid();
        }
        return false;
    });
}

bool Jafg::LEngine::RegisterLevel(LLevel Level)
{
    if (this->IsLevelRegistered(Level.Identifier))
    {
        return false;
    }

    LOG_VERBOSE(LogEngine, "Registering level [{}].", Level.Identifier)
    this->RegisteredLevels.emplace_back(std::move(Level));

    return true;
}

Jafg::LWorld& Jafg::LEngine::Browse(Detail::LWorldTrack& Track, LString Url, Detail::LWorldTrack::LCallbacks Callbacks)
{
    check(Track.IsValid())
    check(!Track.IsWaitingForTravel())

    LOG_VERBOSE(LogEngine, "[{}]: Browsing world to [{}].", Track.ChildWorld->GetHumanReadableName(), Url)

    if (!this->IsTrackUrlInternal(Url))
    {
        // TODO: Not implemented.
        std::unreachable();
    }

    if (auto Idx{Url.find('?')}; Idx != LString::npos)
    {
        if (const LString LevelUrl{ Url.substr(0, Idx) }; this->IsLevelRegistered(LevelUrl) == false)
        {
            LOG_FATAL(LogEngine, "Level [{}] is not registered. Retrieved from URL [{}].", LevelUrl, Url)
        }
    }
    else
    {
        if (this->IsLevelRegistered(Url) == false)
        {
            LOG_FATAL(LogEngine, "Level [{}] is not registered.", Url)
        }
    }

    Track.TravelUrl = std::move(Url);
    Track.Callbacks = std::move(Callbacks);

    return Track.GetWorld();
}

bool Jafg::LEngine::IsTrackUrlInternal(LString const& Url) const
{
    if (Url.empty())
    {
        return true;
    }

    /* We have to implement this in the future. If not internal, then connect to a remote server. */
    return true;
}

bool Jafg::LEngine::TravelTrack(Detail::LWorldTrack& Track)
{
    check(Track.ChildWorld.get())
    check(Track.IsWaitingForTravel())

    LOG_INFO(LogEngine, "Traveling [{}] to [{}].", Track.ChildWorld->GetHumanReadableName(), Track.TravelUrl)

    LLevel* Level{this->GetLevelByInternalUrl(Track.TravelUrl)};
    if (Level == nullptr)
    {
        LOG_ERROR(LogEngine, "Failed to resolve URL for any world [{}].", Track.TravelUrl)
        algo::orphan(&Track.TravelUrl);
        return false;
    }

    if (Track.ChildWorld->GetWorldState() == EWorldState::Running)
    {
        LString OldHumanReadableName{Track.ChildWorld->GetHumanReadableName()};
        Track.ChildWorld = std::make_unique<LWorld>(std::move(OldHumanReadableName));
    }

    check(Track.ChildWorld->GetWorldState() == EWorldState::PreInitializing)
    check(Track.ChildWorld->GetHumanReadableName().empty() == false)

    Track.ChildWorld->InitializeWorld(*Level, std::move(Track.TravelUrl));
    check(Track.TravelUrl.empty())

    return true;
}

Jafg::LLevel* Jafg::LEngine::GetLevelByInternalUrl(LString const& Url)
{
    if (const auto Barrier{ Url.find('?') }; Barrier == LString::npos)
    {
        return algo::find_pointer(this->RegisteredLevels, Url, &LLevel::Identifier);
    }
    else
    {
        return algo::find_pointer(this->RegisteredLevels, Url.substr(0, Barrier), &LLevel::Identifier);
    }
}

#if JAFG_WITH_FOREIGN_SUPPORT
void Jafg::LEngine::RefetchPlugins(TArray<LString> const& AdditionalPaths)
{
    check(Tasks::IsOnMasterThread())

    this->FetchPlugins(App::GetEngineRootRelativeBinaryPath());
    for (const LString& AdditionalPath: AdditionalPaths)
    {
        this->FetchPlugins(LPath{AdditionalPath});
    }
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::LoadPlugin(LString const& Name)
{
    check( Tasks::IsOnMasterThread() )

    LFetchedPlugin const* P{ algo::find_pointer(this->FetchedPlugins, Name, &LFetchedPlugin::Identifier) };
    if (P == nullptr)
    {
        P = algo::find_pointer(this->FetchedPlugins, LPath{Name}, &LFetchedPlugin::AbsolutePath);
    }

    if (P == nullptr)
    {
        return EPluginLoadReturnCode::NotFound;
    }

    return this->LoadPluginImpl(*P);
}

void Jafg::LEngine::LoadPluginNoFailure(LString const& Name)
{
    const EPluginLoadReturnCode::Type ReturnCode = this->LoadPlugin(Name);
    if (ReturnCode == EPluginLoadReturnCode::Success)
    {
        return;
    }

    panicMsgf( "Failed to load plugin [{}] with return code [{}].", Name, LexToString(ReturnCode) )
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::UnLoadPlugin(const LString& InName, const EPluginShutdownReason::Type InReason)
{
    LFetchedPlugin const* P{ algo::find_pointer(this->FetchedPlugins, InName, &LFetchedPlugin::Identifier) };
    if (P == nullptr)
    {
        P = algo::find_pointer(this->FetchedPlugins, LPath{InName}, &LFetchedPlugin::AbsolutePath);
    }

    if (P == nullptr)
    {
        return EPluginLoadReturnCode::NotFound;
    }

    if (LLoadedPlugin* L{ algo::find_pointer(this->LoadedPlugins, P->AbsolutePath, &LLoadedPlugin::GetAbsolutePath) })
    {
        return this->UnLoadPlugin(L, InReason);
    }

    return EPluginLoadReturnCode::NotLoaded;
}

void Jafg::LEngine::UnLoadPluginNoFailure(const LString& InName, const EPluginShutdownReason::Type InReason)
{
    check( InName.empty() == false )

    const EPluginLoadReturnCode::Type ReturnCode = this->UnLoadPlugin(InName, InReason);
    if (ReturnCode == EPluginLoadReturnCode::Success)
    {
        return;
    }

    panicMsgf( "Failed to unload plugin [{}] with return code [{}].", InName, LexToString(ReturnCode) )
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::UnLoadPlugin(LLoadedPlugin* Plugin, const EPluginShutdownReason::Type Reason)
{
    STAT_CYCLE_FUNCTION()

    check(Tasks::IsOnMasterThread())
    check(Plugin)
    check(Plugin->IsLoaded())
    check(Plugin->Fetched.bDynUnloadable)

    const LString CachedIdent{Plugin->GetIdentifier()};
    const LPath CachedPath{Plugin->GetAbsolutePath()};

    Plugin->PrepareLibraryClose(Reason);

    for (auto& Outer : this->KnownOuters)
    {
        Outer->KillEmployeesFromForeignPlugin(Plugin->GetHandle());
    }

    auto RemovedPackages{Detail::GetGlobalCxxRecordRegistry().RemovePackagesOf(Plugin->GetHandle())};
    LOG_VERBOSE(LogForeign, "Removed [{}] registered packages from [{}].", RemovedPackages, CachedIdent)

    const EPluginLoadReturnCode::Type Rc{ Plugin->CloseLibrary(Reason) };
    if (Rc == EPluginLoadReturnCode::Success)
    {
        LOG_INFO(LogForeign, "Successfully unloaded plugin [{}] from [{}].", CachedIdent, CachedPath)
    }

    if (auto const Removed{algo::erase(&this->LoadedPlugins, CachedPath, &LLoadedPlugin::GetAbsolutePath)}; Removed != 1)
    {
        LOG_ERROR(LogForeign, "Suspicious behavior while unloading plugin [{}]. Found [{}] loaded.", CachedIdent, Removed)
    }

    return Rc;
}

void Jafg::LEngine::UnLoadPluginNoFailure(LLoadedPlugin* InPlugin, const EPluginShutdownReason::Type InReason)
{
    check( InPlugin )

    const EPluginLoadReturnCode::Type ReturnCode = this->UnLoadPlugin(InPlugin, InReason);
    if (ReturnCode == EPluginLoadReturnCode::Success)
    {
        return;
    }

    panicMsgf( "Failed to unload plugin [{}] with return code [{}].", InPlugin->GetIdentifier(), LexToString(ReturnCode) )
}

void Jafg::LEngine::FetchPlugins(LPath const& Path)
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogForeign, "[{}]: Fetching.", Path)

    i32 Fetched{};
    for(TArray Files{finder::retrieve_files<finder::recursive_directory_iterator>(Path, ".*\\manifest.jafg")};
        auto const& File: Files)
    {
        if (this->FetchPlugin(File))
        {
            ++Fetched;
        }
    }

    if (Fetched > 0)
    {
        LOG_VERBOSE(LogForeign, "[{}]: Fetched [{}] plugins.", Path, Fetched)
    }
}

bool Jafg::LEngine::FetchPlugin(LPath const& Path)
{
    check(is_regular_file(Path))

    //# Do not use {} init, as this would trigger an implicit creation of an array...
    const json PluginJson = json::parse(finder::read_file(Path), nullptr, false);

    if (PluginJson.is_discarded())
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not valid json. Failed to fetch.", Path)
        return false;
    }

    if (PluginJson.is_object() == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] is not a json object. Failed to fetch.", Path)
        return false;
    }

    if (PluginJson.contains("Version") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain a version. Failed to fetch.", Path)
        return false;
    }

    if (PluginJson.contains("Identifier") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain an identifier. Failed to fetch.", Path)
        return false;
    }

    if (PluginJson.contains("Bin") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain a bin. Failed to fetch.", Path)
        return false;
    }

    LFetchedPlugin P;

    P.AbsolutePath = absolute(Path);
    check( P.AbsolutePath.empty() == false )

    P.Version = PluginJson["Version"].get<std::string>();
    if (P.Version.empty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plugin [{}]. No version.", P.AbsolutePath)
        return false;
    }

    P.Identifier = PluginJson["Identifier"].get<std::string>();
    if (P.Identifier.empty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plugin [{}]. No identifier.", P.AbsolutePath)
        return false;
    }

    if (PluginJson.contains("LifetimeIdentifier"))
    {
        P.LifetimeIdentifier = PluginJson["LifetimeIdentifier"].get<std::string>();
    }
    if (P.LifetimeIdentifier.empty())
    {
        P.LifetimeIdentifier = P.Identifier;
    }
    check( P.LifetimeIdentifier.empty() == false )

    if (PluginJson.contains("NativeIdentifier"))
    {
        P.NativeIdentifier = PluginJson["NativeIdentifier"].get<std::string>();
    }
    if (P.NativeIdentifier.empty())
    {
        P.NativeIdentifier = P.Identifier;
    }
    check( P.NativeIdentifier.empty() == false )

    if (PluginJson.contains("FriendlyName"))
    {
        P.FriendlyName = PluginJson["FriendlyName"].get<std::string>();
    }
    if (P.FriendlyName.empty())
    {
        P.FriendlyName = P.Identifier;
    }
    check( P.FriendlyName.empty() == false )

    if (PluginJson.contains("Description"))
    {
        P.Description = PluginJson["Description"].get<std::string>();
    }

    if (PluginJson.contains("Author"))
    {
        P.Author = PluginJson["Author"].get<std::string>();
    }
    if (P.Author.empty())
    {
        P.Author = "Anonymous";
    }

    if (PluginJson.contains("bDynUnloadable"))
    {
        P.bDynUnloadable = PluginJson["bDynUnloadable"].get<bool>();
    }

    P.Bin = PluginJson["Bin"].get<std::string>();
    if (P.Bin.empty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plugin [{}]. No binary found.", P.Bin)
        return false;
    }

    if (algo::contains(this->FetchedPlugins, P.AbsolutePath, &LFetchedPlugin::AbsolutePath))
    {
        LOG_ERROR(LogForeign, "Plugin [{}] already fetched.", P.AbsolutePath)
        return false;
    }

    if (auto It{ algo::find(this->FetchedPlugins, P.Identifier, &LFetchedPlugin::Identifier) }; It != this->FetchedPlugins.end())
    {
        LOG_ERROR(LogForeign, "Plugin [{}] already fetched from [{}]. Cannot fetch from [{}].", It->Identifier, It->AbsolutePath, P.AbsolutePath)
        return false;
    }

    LOG_VERBOSE(LogForeign, "Fetched plugin [{}] from [{}].", P.Identifier, P.AbsolutePath)
    this->FetchedPlugins.emplace_back(std::move(P));
    return true;
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::LoadPluginImpl(LFetchedPlugin const& FetchedPlugin)
{
    STAT_CYCLE_FUNCTION()

    check( FetchedPlugin.Identifier.empty() == false )

    check( Tasks::IsOnMasterThread() )

    const LPath Bin{ FetchedPlugin.AbsolutePath.parent_path() / FetchedPlugin.Bin };
    check( Bin.is_absolute() )

    if (is_regular_file(Bin) == false)
    {
        LOG_ERROR(LogForeign, "Expected [{}] to exist.", FetchedPlugin.Identifier, Bin)
        return EPluginLoadReturnCode::NoBin;
    }

    if (algo::contains(this->LoadedPlugins, Bin, &LLoadedPlugin::GetPathToBin))
    {
        return EPluginLoadReturnCode::AlreadyLoaded;
    }

    LOG_VERBOSE(LogForeign, "Loading plugin [{}] from [{}].", FetchedPlugin.Identifier, Bin)

    check(Detail::GetGlobalCxxRecordRegistry().GetPendingPackages().empty())
    Detail::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(true);

    LLoadedPlugin Plugin{ FetchedPlugin, Bin };
    check( Plugin.GetIdentifier().empty() == false )
    const EPluginLoadReturnCode::Type Rc{ Plugin.OpenLibrary() };
    Detail::GetGlobalCxxRecordRegistry().SetAllowNewPendingPackages(false);

    if (Rc != EPluginLoadReturnCode::Success)
    {
        Detail::GetGlobalCxxRecordRegistry().KillPendingPackages();
        Detail::GetGlobalCarnifex().KillAllGarbageChildren();
        return Rc;
    }

    LOG_VERBOSE(LogForeign, "Loaded plugin [{}] from [{}]. Now loading contents.", Plugin.GetIdentifier(), Plugin.GetPathToBin())

    Plugin.Uuid = this->GetNextPluginUuid();

    Detail::GetGlobalCxxRecordRegistry().LoadPendingPackages(Plugin.GetHandle());

    Plugin.Lifetime->OnFinishedLoading();

    this->LoadedPlugins.emplace_back(std::move(Plugin));
    auto& Ref{ this->LoadedPlugins.back() };
    this->OnForeignPluginLoaded.Broadcast(Ref);

    LOG_INFO(LogForeign, "Successfully loaded plugin [{}] from [{}].", Ref.GetIdentifier(), Ref.GetPathToBin())

    return EPluginLoadReturnCode::Success;
}
#endif /* JAFG_WITH_FOREIGN_SUPPORT */

#if JAFG_WITH_REST_CLS
void Jafg::LEngine::SetReSTCliCorePaths()
{
    LOG_VERBOSE(LogReST, "Setting ReST CLI core paths...")

    this->ReSTCli.Get("/info", [](ReST::LRequest const&, ReST::LResponse* OutResponse) -> void
    {
        check(OutResponse)
        check(GEngine)

        json Info;
        Info["BuildTime"] = App::BuildTime();
        Info["BuildDate"] = App::BuildDate();
        Info["VcsBranch"] = App::BuildVcsBranch();
        Info["VcsRevision"] = App::BuildVcsRevision();

        Info["EngineVersion"] = App::EngineVersion().ToString();

        Info["CompilerVersion"] = App::CompilerVersion();
        Info["CxxStandard"] = App::CxxStandard();

        Info["TargetPlatform"] = App::GetTargetPlatform();
        Info["TargetArchitecture"] = App::GetTargetArchitecture();
        Info["TargetType"] = App::GetTargetType();
        Info["TargetConfig"] = App::GetTargetConfiguration();
        Info["bEverRender"] = GEngine->CanEverRender();

        Info["Uptime"] = GEngine->FrameStartElapsedTime;
        Info["Ticks"] = GEngine->FrameCount;
        Info["AvgDeltaTime"] = algo::time_diff(GEngine->PreviousStat.Start, GEngine->CurrentStat.Start) / static_cast<f64>(GEngine->PreviousStat.FrameCount);
        Info["AvgTickRate"] = static_cast<f64>(GEngine->PreviousStat.FrameCount) / algo::time_diff(GEngine->PreviousStat.Start, GEngine->CurrentStat.Start);
        Info["MaxDeltaTime"] = LEngine::MaxDeltaTime;
        Info["LowestDeltaTime"] = GEngine->PreviousStat.Low;
        Info["HighestDeltaTime"] = GEngine->PreviousStat.High;
        Info["HighestLostDeltaTime"] = GEngine->PreviousStat.HighestLoss;
        Info["HighestIdleTime"] = GEngine->PreviousStat.HighestIdle;
        Info["bTracerPid"] = App::IsTracerPidValid();
        Info["bEverProfile"] = App::CanEverProfile();
        Info["bProfiling"] = App::IsAllowProfiling();

        OutResponse->SetContent(Info.dump(), "application/json");

        return;
    });

    this->ReSTCli.Get("/", [](ReST::LRequest const&, ReST::LResponse* OutResponse) -> void
    {
        check(OutResponse)

        OutResponse->SetStatusCode(ReST::Found_302);
        OutResponse->AddHeader("Location", "/info");
        OutResponse->SetContent("Redirecting to /info", "text/plain");

        return;
    });

    this->ReSTCli.Get("/logs", [](ReST::LRequest const& Request, ReST::LResponse* OutResponse) -> void
    {
        check(OutResponse)

#if JAFG_SAVE_LOGS_IN_MEMORY
        if (Request.HasParameter("id"))
        {
            LString Id{ Request.GetParameter("id") };

            if (Id.empty())
            {
                OutResponse->SetStatusCode(ReST::BadRequest_400);
                OutResponse->SetContent(R"({"error":"Empty 'id' parameter."})", "application/json");
                return;
            }

            u64 ConvertedId;
            if (serde::FromStringRelaxed(&ConvertedId, Id) == false)
            {
                OutResponse->SetStatusCode(ReST::BadRequest_400);
                OutResponse->SetContent(R"({"error":"Invalid 'id' parameter."})", "application/json");
                return;
            }

            json Res;
            auto& Logs = Res["logs"] = json::array();
            std::unique_lock Lock{ Detail::GLongLiquidLogsMutex };
            while (ConvertedId < Detail::GLongLiquidLogs.size())
            {
                auto const& Entry{ Detail::GLongLiquidLogs.at(ConvertedId) };

                json E;
                E["id"] = ConvertedId;
                E["level"] = std::get<0>(Entry);
                E["message"] = std::get<1>(Entry);
                Logs.emplace_back(std::move(E));

                ++ConvertedId;
                continue;
            }

            OutResponse->SetContent(Res.dump(), "application/json");
        }
        else
        {
            OutResponse->SetStatusCode(ReST::BadRequest_400);
            OutResponse->SetContent(R"({"error":"Missing 'id' parameter."})", "application/json");
        }
#else /* JAFG_SAVE_LOGS_IN_MEMORY */
        OutResponse->SetStatusCode(ReST::BadRequest_400);
        OutResponse->SetContent(R"({"error":"Logs are not permitted in this engine build."})", "application/json");
#endif /* !JAFG_SAVE_LOGS_IN_MEMORY */

        return;
    });
}

void Jafg::LEngine::StartReSTCliServer()
{
    if (GetSingleton<JReSTCliPreferences>().bAlwaysDisable)
    {
        LOG_WARNING(LogReST, "ReST CLI server is always disabled by preferences.")
        return;
    }

    LOG_VERBOSE(LogEngine, "Starting ReST CLI server...")

    if (Tasks::HasReSTCliThread())
    {
        LOG_WARNING(LogEngine, "ReST CLI server is already running.")
        return;
    }

    if
    (
        ETaskExit::Type const Rc{ Tasks::Private::LaunchNamedThread(ENamedThreads::ReSTCli, &this->ReSTCli, false) };
        Rc != ETaskExit::Success
    )
    {
        LOG_FATAL(LogEngine, "Failed to create ReST Cli thread: [{}].", static_cast<i32>(Rc))
    }
}

void Jafg::LEngine::StopReSTCliServer(ERunnableStopReason::Type Reason /* = ERunnableStopReason::EngineTermination */)
{
    LOG_VERBOSE(LogEngine, "Trying to stop ReST CLI server...")
    this->ReSTCli.Stop(Reason);
    this->ReSTCli.Join();
}
#endif /* JAFG_WITH_REST_CLS */
