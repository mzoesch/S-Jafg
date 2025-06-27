// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Engine/CoreGlobals.h"
#include "Async/TaskUtility.h"
#include "Cli/CliExtended.h"
#include "Cli/CliPrimitives.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Subsystems/EngineSubsystem.h"
#include "Cli/CommandLineInterface.h"
#include "Engine/Carnifex.h"
#include "Stats/Stats.h"
#include "Platform/PlatformMisc.h"
#include "System/Paths.h"
#if LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    #if LAL_WITH_CLANG
        #pragma clang diagnostic push
        #pragma clang diagnostic ignored "-W#warnings"
    #endif /* LAL_WITH_CLANG */
    #include "nlohmann/json.hpp"
    #if LAL_WITH_CLANG
        #pragma clang diagnostic pop
    #endif /* LAL_WITH_CLANG */
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

ENGINE_API Jafg::LEngine* GEngine { nullptr };

namespace Jafg
{

ENGINE_API bool bGShouldRequestExit { false };
ENGINE_API bool bGEngineRequestingExit { false };

ENGINE_API i32     GCustomExitStatusOverride { INDEX_NONE };
ENGINE_API LString GCustomExitReason;

} /* ~Namespace Jafg */

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

bool Jafg::LWorldStorage::IsValid() const noexcept
{
    if (GEngine == nullptr)
    {
        return false;
    }

    return GEngine->IsWorldValid(this->World);
}

void Jafg::LEngine::Initialize()
{
    STAT_CYCLE_FUNCTION()

    Private::AddPrimitivesToCli(this->GetCommandLineInterface());
    Private::AddExtendedPrimitivesToCli(this->GetCommandLineInterface());

    /* Engine stuff. */
    {
        ensure(this->CommandLineInterface.RegisterType({"World", "A world registered to the engine.", "",
        [](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.IsEmpty())
            {
                return false;
            }

            if (GEngine == nullptr)
            {
                return false;
            }

            const bool bValid { GEngine->GetContexts().FindByPredicate([Args, Cursor](const Private::LWorldContext& InContext) -> bool
            {
                return InContext.ChildWorld->GetHumanReadableName() == Args[*Cursor].Name;
            }) != INDEX_NONE };
            if (bValid)
            {
                ++*Cursor;
            }

            return bValid;
        },
        nullptr,
        [](const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions) -> TArray<LString>
        {
            const LCommandArgs* Target { nullptr };

            if (Args.SubArgs.IsValidIndex(Cursor))
            {
                Target = &Args[Cursor];
            }

            if (GEngine == nullptr)
            {
                return { };
            }

            TArray<LString> Out;

            for (const Private::LWorldContext& Context : GEngine->GetContexts())
            {
                if (Out.GetSize() >= MaxSuggestions)
                {
                    break;
                }

                check( Context.ChildWorld )

                if (Target)
                {
                    if (Context.ChildWorld->GetHumanReadableName().StartsWith(Target->Name))
                    {
                        Out.Emplace(Context.ChildWorld->GetHumanReadableName());
                    }
                }
                else
                {
                    Out.Emplace(Context.ChildWorld->GetHumanReadableName());
                }

                continue;
            }

            return Out;
        }}).IsValid());

        ensure(this->CommandLineInterface.RegisterCommand({"Set", "Set any variable.",
        LCommandParams{}
        .Token(LCliType::Type<LCliVariable>())
        .Token(LCliType
        {
            "VarType", "The value to set.",
            nullptr,
            [](const LCommandArgs& Args, i32* Cursor) -> bool
            {
                checkSlow( *Cursor < Args.GetArgCount() )
                if (Args[*Cursor].Name.IsEmpty())
                {
                    return false;
                }

                ++*Cursor;
                return true;
            },
            nullptr,
            [](const LCommandArgs& Args, const i32 Cursor, const i32 MaxSuggestions) -> TArray<LString>
            {
                if (Args.SubArgs.IsValidIndex(Cursor - 1) == false)
                {
                    LOG_WARNING(LogCli, "Encountered invalid command args [{}].", Args.GetCatRepresentation())
                    return { };
                }

                if (GEngine == nullptr)
                {
                    return { };
                }

                if
                (
                    const LCliVariable* TargetVar { GEngine->CommandLineInterface.GetVariable(Args[Cursor - 1].Name) };
                    TargetVar
                )
                {
                    check( TargetVar->GetType() )
                    return TargetVar->GetType()->Suggest(Args, Cursor, MaxSuggestions);
                }

                return { };
            }
        })
        .Exec([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
        {
            check( InArgs.GetArgCount() == 2 )
            if (LCliVariable* Var = GEngine->CommandLineInterface.GetVariable(InArgs[0].Name); Var)
            {
                i32 Cursor = 1;
                if (Var->GetType()->CanParse(InArgs, &Cursor) == false)
                {
                    OutResponse->Rc = ECommandReturnCode::TypeError;
                    OutResponse->StdOut = LString::SprintF("Cannot parse [{}] as [{}]", InArgs[1].Name, Var->GetType()->GetIdentifier());
                    return;
                }

                if (Var->SetValue(InArgs[1].Name))
                {
                    OutResponse->Rc = ECommandReturnCode::Success;
                    OutResponse->StdOut = LString::SprintF("Updated [{}] to [{}]", Var->GetIdentifier(), Var->GetValue());
                }
                else
                {
                    OutResponse->Rc = ECommandReturnCode::Success;
                }
            }
            else
            {
                OutResponse->Rc = ECommandReturnCode::SemanticError;
                OutResponse->StdOut = LString::SprintF("No such variable [{}]", InArgs[0].Name);
            }

            return;
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterCommand({"Get", "Get any variable.",
        LCommandParams{}
        .Token(LCliType::Type("Var", "The variable to get."))
        .Exec([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
        {
            check( InArgs.GetArgCount() == 1 )
            if (const LCliVariable* Var = GEngine->CommandLineInterface.GetVariable(InArgs[0].Name); Var)
            {
                OutResponse->Rc = ECommandReturnCode::Success;
                OutResponse->StdOut = LString::SprintF("[{}] == [{}]", Var->GetIdentifier(), Var->GetValue());
            }
            else
            {
                OutResponse->Rc = ECommandReturnCode::SemanticError;
                OutResponse->StdOut = LString::SprintF("No such variable [{}]", InArgs[0].Name);
            }

            return;
        })}).IsValid());
    }

    this->ObjectContext.SetHumanReadableName("Engine");
    this->Collection.DeferredInitialize(&this->ObjectContext, true);
    this->Collection.InitializeSubsystems(JEngineSubsystem::StaticClass());

#if WITH_LOCAL_LAYER
    check( this->LocalEgo.IsValid() == false )
    this->LocalEgo.Initialize();
#endif /* WITH_LOCAL_LAYER */

    return;
}

void Jafg::LEngine::Tick(const f32 DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, 5);

#if WITH_LOCAL_LAYER
    this->LocalEgo.Tick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

    for (Private::LWorldContext& Context : this->Contexts)
    {
        bool bTraveled { false };

        if (Context.IsWaitingForTravel())
        {
            if (this->TravelContext(Context))
            {
                bTraveled = true;
            }
        }

        checkSlow( Context.IsValid() )

        if (bTraveled == false && Context.ChildWorld->CanTick())
        {
            Context.ChildWorld->Tick(DeltaTime);
        }

        continue;
    }

#if WITH_LOCAL_LAYER
    this->LocalEgo.OnLateTick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

#if WITH_LOCAL_LAYER
    for (Private::LWorldContext& Context : this->Contexts)
    {
        checkSlow( Context.IsValid() )

        if (Context.ChildWorld->CanTick())
        {
            Context.ChildWorld->LateTick(DeltaTime);
        }

        continue;
    }
#endif /* WITH_LOCAL_LAYER */

    Tasks::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, 5);

    return;
}

void Jafg::LEngine::TearDown()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogEngine, "Tearing down engine.")

    LOG_VERBOSE(LogEngine, "Deallocating {} registered contexts.", this->Contexts.GetSize())
    for (const Private::LWorldContext& Context : this->Contexts)
    {
        Context.ChildWorld->TearDownContext();
        check( Context.ChildWorld->GetWorldState() == EWorldState::WaitingForKill )
        continue;
    }
    for (Private::LWorldContext& Context : this->Contexts)
    {
        check( Context.ChildWorld->GetWorldState() == EWorldState::WaitingForKill )
        delete Context.ChildWorld;
        Context.ChildWorld = nullptr;
    }
    this->Contexts.Empty();

    this->Collection.TearDownSubsystems();
    this->ObjectContext.TearDownContext();

#if WITH_LOCAL_LAYER
    if (ensure(this->LocalEgo.IsValid()))
    {
        this->LocalEgo.TearDown();
    }
#endif /* WITH_LOCAL_LAYER */

    Tasks::Private::StopAndJoinRemainingThreads();

    LOG_VERBOSE(LogJafgInternal, "Deallocating  {} registered levels.", this->RegisteredLevels.GetSize())
    this->RegisteredLevels.Empty();

    this->CommandLineInterface.TearDown();

    Private::GCarnifexReferrer->KillAllGarbageChildren();

#if LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
    if (this->LoadedPlugins.IsEmpty() == false)
    {
        LOG_VERBOSE(LogForeign, "There are [{}] loaded plugins. Unloading them now.", this->LoadedPlugins.GetSize())

        for (i32 i = 0; i < this->LoadedPlugins.GetSize(); ++i)
        {
            const LString CachedIdentifier = this->LoadedPlugins[i].GetIdentifier();
            if
            (
                const EPluginLoadReturnCode::Type Rc = this->UnLoadPlugin(&this->LoadedPlugins[i], EPluginShutdownReason::EngineTearDown);
                Rc != EPluginLoadReturnCode::Success
            )
            {
                LOG_WARNING(LogForeign, "Failed to unload plugin [{}] with return code [{}].", CachedIdentifier, LexToString(Rc) )
            }

            continue;
        }

        check( this->LoadedPlugins.IsEmpty() )
    }
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */

    this->UnregisterObjectContext(GOmniVitaContext);
    if (this->KnownObjectContexts.IsEmpty() == false)
    {
        LOG_WARNING(LogObjectInternal, "Some context [#{}] where not correctly teared down.", this->KnownObjectContexts.GetSize() )
    }

    return;
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::BeginExitIfRequested()
{
    ::Jafg::Private::BeginExitIfRequested();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::ReflectForwardedExitRequest()
{
    ::Jafg::Private::ReflectForwardEngineExitRequest();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit()
{
    ::Jafg::RequestEngineExit();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const LString& Reason)
{
    ::Jafg::RequestEngineExit(Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const i32 CustomExitStatus)
{
    ::Jafg::RequestEngineExit(CustomExitStatus);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const i32 CustomExitStatus, const LString& Reason)
{
    ::Jafg::RequestEngineExit(CustomExitStatus, Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot check for rendering state. */
// ReSharper disable once CppMemberFunctionMayBeStatic
bool Jafg::LEngine::CanEverRender() const noexcept
{
#if WITH_FRONTEND
    return true;
#else /* WITH_FRONTEND */
    return false;
#endif /* !WITH_FRONTEND */
}

void Jafg::LEngine::RegisterObjectContext(LObjectContext* InContext)
{
    if (this->KnownObjectContexts.Contains(InContext))
    {
        panic( "Context already registered." )
        return;
    }

    this->KnownObjectContexts.Emplace(InContext);
    return;
}

void Jafg::LEngine::UnregisterObjectContext(LObjectContext* InContext)
{
    if (this->KnownObjectContexts.Contains(InContext) == false)
    {
        panic( "Context not registered." )
        return;
    }

    this->KnownObjectContexts.RemoveOnceChecked(InContext);
    return;
}

Jafg::Private::LWorldContext& Jafg::LEngine::GetContextFromWorld(const LWorld* World)
{
    check( World )

    for (Private::LWorldContext& Context : this->Contexts)
    {
        if (Context.ChildWorld == World)
        {
            return Context;
        }

        continue;
    }

    jassertNoEntry()
    abort();
}

Jafg::LWorldStorage Jafg::LEngine::SummonWorld(const LString& HumanReadableName)
{
    const Private::LWorldContext& Context = this->CreateNewWorldContext(HumanReadableName);
    return LWorldStorage(Context.ChildWorld);
}

bool Jafg::LEngine::IsWorldValid(const LWorld* InWorld) const
{
    return this->Contexts.FindByPredicate([InWorld](const Private::LWorldContext& Context) -> bool
    {
        if (Context.ChildWorld == InWorld)
        {
            return Context.IsValid();
        }

        return false;
    });
}

void Jafg::LEngine::Browse(const LWorld* World, const LString& Url)
{
    this->Browse(this->GetContextFromWorld(World), Url);
}

bool Jafg::LEngine::RegisterLevel(const LLevel& InLevel)
{
    if (this->IsLevelRegistered(InLevel.Identifier))
    {
        return false;
    }

    this->RegisteredLevels.Add(InLevel);

    return true;
}

bool Jafg::LEngine::RegisterLevel(LLevel&& InLevel)
{
    if (this->IsLevelRegistered(InLevel.Identifier))
    {
        return false;
    }

    this->RegisteredLevels.Emplace(std::move(InLevel));

    return true;
}

bool Jafg::LEngine::IsLevelRegistered(const LString& Identifier) const
{
    return this->RegisteredLevels.Contains(Identifier);
}

Jafg::Private::LWorldContext& Jafg::LEngine::CreateNewWorldContext(const LString& InHumanReadableName)
{
    check( Tasks::IsOnMasterThread() )
    this->Contexts.Emplace(InHumanReadableName);
    return *this->Contexts.GetLast();
}

void Jafg::LEngine::Browse(Private::LWorldContext& Context, const LString& Url) const
{
    if (this->IsContextUrlInternal(Url) == false)
    {
        unimplemented()
        return;
    }

    LString Level;
    if (const i32 Barrier = Url.FindFirst("?"); Barrier == INDEX_NONE)
    {
        if (this->IsLevelRegistered(Url) == false)
        {
            panicMsgf( "Local Url [{}] is not registered.", Url )
            return;
        }
    }
    else
    {
        if (this->IsLevelRegistered(Url.LeftChop(Barrier)) == false)
        {
            panicMsgf( "Local Url [{}] is not registered.", Url )
            return;
        }
    }

    Context.TravelUrl = Url;

    return;
}

bool Jafg::LEngine::IsContextUrlInternal(const LString& Url) const
{
    if (Url.IsEmpty())
    {
        return false;
    }

    /* We have to implement this in the future. If not internal, then connect to a remote server. */
    return true;
}

bool Jafg::LEngine::TravelContext(Private::LWorldContext& Context)
{
    check( Context.IsWaitingForTravel() )

    LOG_INFO(LogEngine, "Traveling world to [{}].", Context.TravelUrl)

    LLevel* Level = this->GetLevelByInternalUrl(Context.TravelUrl);
    if (Level == nullptr)
    {
        LOG_ERROR(LogEngine, "Failed to resolve URL for any world [{}].", Context.TravelUrl)
        Context.TravelUrl.Empty();
        return false;
    }

    check( Context.ChildWorld )

    if (Context.ChildWorld->GetWorldState() == EWorldState::Running)
    {
        Context.ChildWorld->TearDownContext();
    }

    jassert
    (
           Context.ChildWorld->GetWorldState() == EWorldState::Uninitialized
        || Context.ChildWorld->GetWorldState() == EWorldState::WaitingForKill
        && "Travel is only allowed in these world states."
    )

    Context.ChildWorld->InitializeWorld(*Level, std::move(Context.TravelUrl));

    check( Context.TravelUrl.IsEmpty() )

    return true;
}

Jafg::LLevel* Jafg::LEngine::GetLevelByInternalUrl(const LString& Url)
{
    if (const i32 Barrier = Url.FindFirst("?"); Barrier == INDEX_NONE)
    {
        return this->RegisteredLevels.FindRef(Url);
    }
    else
    {
        return this->RegisteredLevels.FindRef(Url.LeftChop(Barrier));
    }
}

#if LAL_PLATFORM_SUPPORTS_SHARED_LIBRARIES
void Jafg::LEngine::RefetchPlugins(const TArray<LString>& InAdditionalPaths)
{
    check( Tasks::IsOnMasterThread() )

    this->FetchPlugins(GPlatformMisc->RealEngineRootDir);

    for (const LString& AdditionalPath : InAdditionalPaths)
    {
        this->FetchPlugins({AdditionalPath.GetBegin()});
    }

    return;
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::LoadPlugin(const LString& InName)
{
    check( Tasks::IsOnMasterThread() )

    const LFetchedPlugin* P = this->FetchedPlugins.FindRefByPredicate([InName](const LFetchedPlugin& Plugin) -> bool
    {
       return Plugin.Identifier == InName;
    });
    if (P == nullptr)
    {
        P = this->FetchedPlugins.FindRefByPredicate([InName](const LFetchedPlugin& Plugin) -> bool
        {
            return Plugin.AbsolutePath == InName.ToPtr();
        });
    }

    if (P == nullptr)
    {
        return EPluginLoadReturnCode::NotFound;
    }

    return this->LoadPluginImpl(*P);
}

void Jafg::LEngine::LoadPluginNoFailure(const LString& InName)
{
    const EPluginLoadReturnCode::Type ReturnCode = this->LoadPlugin(InName);
    if (ReturnCode == EPluginLoadReturnCode::Success)
    {
        return;
    }

    panicMsgf( "Failed to load plugin [{}] with return code [{}].", InName, LexToString(ReturnCode) )

    return;
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::UnLoadPlugin(const LString& InName, const EPluginShutdownReason::Type InReason)
{
    const LFetchedPlugin* P = this->FetchedPlugins.FindRefByPredicate([InName](const LFetchedPlugin& Plugin) -> bool
    {
       return Plugin.Identifier == InName;
    });
    if (P == nullptr)
    {
        P = this->FetchedPlugins.FindRefByPredicate([InName](const LFetchedPlugin& Plugin) -> bool
        {
            return Plugin.AbsolutePath == InName.ToPtr();
        });
    }

    if (P == nullptr)
    {
        return EPluginLoadReturnCode::NotFound;
    }

    LLoadedPlugin* L = this->LoadedPlugins.FindRefByPredicate([P](const LLoadedPlugin& Plugin) -> bool
    {
        return Plugin.GetAbsolutePath() == P->AbsolutePath;
    });

    if (L)
    {
        return this->UnLoadPlugin(L, InReason);
    }

    return EPluginLoadReturnCode::NotLoaded;
}

void Jafg::LEngine::UnLoadPluginNoFailure(const LString& InName, const EPluginShutdownReason::Type InReason)
{
    check( InName.IsEmpty() == false )

    const EPluginLoadReturnCode::Type ReturnCode = this->UnLoadPlugin(InName, InReason);
    if (ReturnCode == EPluginLoadReturnCode::Success)
    {
        return;
    }

    panicMsgf( "Failed to unload plugin [{}] with return code [{}].", InName, LexToString(ReturnCode) )

    return;
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::UnLoadPlugin(LLoadedPlugin* InPlugin, const EPluginShutdownReason::Type InReason)
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )
    check( InPlugin )

    const LString CachedIdent = InPlugin->GetIdentifier();
    const LString CachedPath  = InPlugin->GetAbsolutePath();

    InPlugin->PrePareLibraryClose(InReason);

    for (LObjectContext* Context : this->KnownObjectContexts)
    {
        if (Context->IsValid() == false)
        {
            LOG_WARNING(LogObjectInternal, "Encountered invalid context while unloading plugin [{}].", CachedIdent)
            continue;
        }

        Context->SeparateAndKillEmployees(InPlugin->GetHandle());

        continue;
    }

    check( this->ForeignContextCursor == nullptr )
    check( InPlugin->ObjectContext && InPlugin->ObjectContext->IsValid() )
    InPlugin->ObjectContext->TearDownContext();
    InPlugin->ObjectContext.Reset();

    const i32 Removed { Private::GObjectRegistry->RemovePackagesOf(InPlugin->GetHandle()) };
    LOG_VERBOSE(LogForeign, "Removed [{}] registered packages from [{}].", Removed, CachedIdent)

    const EPluginLoadReturnCode::Type Rc = InPlugin->CloseLibrary(InReason);

    if (Rc == EPluginLoadReturnCode::Success)
    {
        LOG_INFO(LogForeign, "Successfully unloaded plugin [{}] from [{}].", CachedIdent, CachedPath )
    }

    if (const i32 Removed = this->LoadedPlugins.RemoveByPredicate([&CachedPath](const LLoadedPlugin& LoadedPlugin) -> bool
    {
        return LoadedPlugin.GetAbsolutePath() == CachedPath.ToPtr();
    }); Removed != 1)
    {
        LOG_ERROR(LogForeign, "Suspicious behavior while unloading plugin [{}]. Found [{}] loaded.", CachedIdent, Removed )
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

    return;
}

void Jafg::LEngine::FetchPlugins(const LPath& InPath)
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogForeign, "Fetching in [{}] ...", InPath)

    i32 Fetched { 0 };

    for
    (
        const TArray<LString> Files = Finder::FindFilesRecursively(InPath, true, ".jafg");
        const LString& File: Files
    )
    {
        if (File.EndsWith("/root.plugin.jafg") == false)
        {
            continue;
        }

        if (this->FetchPlugin({File.GetBegin()}))
        {
            ++Fetched;
        }

        continue;
    }

    if (Fetched > 0)
    {
        LOG_VERBOSE(LogForeign, "Fetched [{}] plugins in [{}].", Fetched, InPath)
    }

    return;
}

bool Jafg::LEngine::FetchPlugin(LPath&& InPath)
{
    checkCode( Paths::CheckFile(InPath) )

    using json = nlohmann::json;

    const json PluginJson = json::parse(Paths::ReadFile(InPath).ToPtr());

    if (PluginJson.contains("Identifier") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain an identifier. Failed to fetch.", InPath)
        return false;
    }

    if (PluginJson.contains("FriendlyName") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain a friendly name. Failed to fetch.", InPath)
        return false;
    }

    if (PluginJson.contains("Bin") == false)
    {
        LOG_ERROR(LogForeign, "Plugin [{}] does not contain a bin. Failed to fetch.", InPath)
        return false;
    }

    LFetchedPlugin P =
    {
        std::move(InPath),
        PluginJson["Identifier"].get<std::string>().c_str(),
        PluginJson["FriendlyName"].get<std::string>().c_str(),
        PluginJson["Bin"].get<std::string>().c_str()
    };

    if (P.AbsolutePath.IsEmpty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plug.")
        return false;
    }

    if (P.Identifier.IsEmpty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plug [{}].", P.AbsolutePath)
        return false;
    }

    if (P.FriendlyName.IsEmpty())
    {
        P.FriendlyName = P.Identifier;
    }

    if (P.Bin.IsEmpty())
    {
        LOG_ERROR(LogForeign, "Failed to fetch plug [{}].", P.Bin)
        return false;
    }

    LOG_VERBOSE(LogForeign, "Fetched plugin [{}].", P.FriendlyName)

    this->FetchedPlugins.Emplace(std::move(P));

    return true;
}

Jafg::EPluginLoadReturnCode::Type Jafg::LEngine::LoadPluginImpl(const LFetchedPlugin& InFetchedPlugin)
{
    STAT_CYCLE_FUNCTION()

    LPath PathToBin;

    if (InFetchedPlugin.Bin.IsAbsolute())
    {
        if (Paths::DoesFileExist(InFetchedPlugin.Bin) == false)
        {
            return EPluginLoadReturnCode::NoBin;
        }

        PathToBin = InFetchedPlugin.Bin;
    }
    else
    {
        PathToBin = InFetchedPlugin.AbsolutePath;
        PathToBin.PopSubPath();
        PathToBin /= InFetchedPlugin.Bin;

        if (Paths::DoesFileExist(PathToBin) == false)
        {
            return EPluginLoadReturnCode::NoBin;
        }
    }

    if (this->LoadedPlugins.ContainsByPredicate([&PathToBin](const LLoadedPlugin& LoadedPlugin) -> bool
    {
        return LoadedPlugin.GetPathToBin() == PathToBin;
    }))
    {
        return EPluginLoadReturnCode::AlreadyLoaded;
    }

    LOG_VERBOSE(LogForeign, "Loading plugin [{}] from [{}].", InFetchedPlugin.FriendlyName, PathToBin)

    LLoadedPlugin Plugin(InFetchedPlugin, std::move(PathToBin));

    check( Private::GetRegisterObjectQueue().IsEmpty() )

    if (const EPluginLoadReturnCode::Type Rc = Plugin.OpenLibrary(); Rc != EPluginLoadReturnCode::Success)
    {
        Private::GObjectRegistry->KillPendingPackages();
        return Rc;
    }

    LOG_VERBOSE(LogForeign, "Loaded plugin [{}] from [{}]. Now loading contents.", Plugin.GetIdentifier(), Plugin.GetPathToBin())

    Plugin.Uuid = this->GetNextPluginUuid();

    Plugin.ObjectContext = Smart::EmplaceUnique<LObjectContext>(GlobalCarnifex);
    Plugin.ObjectContext->SetHumanReadableName(LString::SprintF("Plugin_{}", Plugin.GetIdentifier()));

    this->ForeignContextCursor = Plugin.ObjectContext;
    Private::GObjectRegistry->LoadPendingPackages(Plugin.GetHandle());
    this->ForeignContextCursor = nullptr;

    this->OnForeignPluginLoaded.Broadcast(Plugin.ObjectContext);

    LOG_INFO(LogForeign, "Successfully loaded plugin [{}] from [{}].", Plugin.GetIdentifier(), Plugin.GetPathToBin())
    this->LoadedPlugins.Emplace(std::move(Plugin));

    return EPluginLoadReturnCode::Success;
}
#endif /* PLATFORM_SUPPORTS_SHARED_LIBRARIES */
