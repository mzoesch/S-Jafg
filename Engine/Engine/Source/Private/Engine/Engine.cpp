// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/Engine.h"
#include "Engine/CoreGlobals.h"
#include "Async/TaskUtility.h"
#include "Engine/World.h"
#include "User/LocalEgo.h"
#include "Subsystems/EngineSubsystem.h"
#include "Cli/CommandLineInterface.h"
#include "Stats/Stats.h"

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

namespace Jafg
{

ENGINE_API LEngine* GEngine = nullptr;

ENGINE_API bool bGShouldRequestExit = false;
ENGINE_API bool bGEngineRequestingExit = false;

ENGINE_API i32     GCustomExitStatusOverride = INDEX_NONE;
ENGINE_API LString GCustomExitReason;

} /* ~Namespace Jafg. */

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void Jafg::LEngine::Initialize()
{
    STAT_CYCLE_FUNCTION()

    // Register primitives
    {
        STAT_QUICK_CYCLE_START("AddCliPrimitives")

        ensure(this->CommandLineInterface.RegisterType({"Integer", "A 32 bit signed Integer.", "0",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            const LString& String = Args[*Cursor].Name;
            if (String.IsEmpty())
            {
                return false;
            }
            i32 Value;
            auto [ptr, ec] = std::from_chars(
                String.GetBegin(),
                String.GetEnd(),
                Value
            );
            ++*Cursor;
            return ec == std::errc{} && ptr == String.GetEnd();
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"Byte", "A 8 bit unsigned integer.", "0",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            const LString& String = Args[*Cursor].Name;
            if (String.IsEmpty())
            {
                return false;
            }
            i32 Value;
            auto [ptr, ec] = std::from_chars(
                String.GetBegin(),
                String.GetEnd(),
                Value
            );
            ++*Cursor;
            return ec == std::errc{} && ptr == String.GetEnd() && (Value >= 0 && Value <= 255);
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"Float", "A 32 bit floating point number.", "0.0",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            const LString& String = Args[*Cursor].Name;
            if (String.IsEmpty())
            {
                return false;
            }
            f32 Value;
            auto [ptr, ec] = std::from_chars(
                String.GetBegin(),
                String.GetEnd(),
                Value
            );
            ++*Cursor;
            return ec == std::errc{} && ptr == String.GetEnd();
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"String", "A string.", "",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.IsEmpty())
            {
                return false;
            }
            ++*Cursor;
            return true;
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"Bool", "A boolean.", "false",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.IsEmpty())
            {
                return false;
            }

            if
            (
                   Args[*Cursor].Name.Equals("true")  == false
                && Args[*Cursor].Name.Equals("false") == false
                && Args[*Cursor].Name.Equals("1")     == false
                && Args[*Cursor].Name.Equals("0")     == false
            )
            {
                return false;
            }

            ++*Cursor;
            return true;
        }),
        LOnValueSetDelegate::CreateStrong([](const LCommandArgs& InValue, LString* OutValue) -> bool
        {
            check( InValue.IsValid() && InValue.Name.IsEmpty() == false )

            if (InValue.Name.Equals("true") || InValue.Name.Equals("1"))
            {
                if (*OutValue != "true")
                {
                    *OutValue = "true";
                    return true;
                }
            }
            else if (InValue.Name.Equals("false") || InValue.Name.Equals("0"))
            {
                if (*OutValue != "false")
                {
                    *OutValue = "false";
                    return true;
                }
            }

            return false;
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"Any", "Any value.", "",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.IsEmpty())
            {
                return false;
            }
            ++*Cursor;
            return true;
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterType({"Var", "A variable.", "NULL",
        LOnParseTypeDelegate::CreateStrong([](const LCommandArgs& Args, i32* Cursor) -> bool
        {
            checkSlow( *Cursor < Args.GetArgCount() )
            if (Args[*Cursor].Name.IsEmpty())
            {
                return false;
            }
            ++*Cursor;
            return true;
        })}).IsValid());

        ensure(this->CommandLineInterface.RegisterCommand({"Set", "Set any variable.",
        LCommandParams()
        .AddToken(LCliType::Type("Var", "The variable to set."))
        .AddToken(LCliType::Type("Any", "The value to set."))
        .SetExec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
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
        }))}).IsValid());

        ensure(this->CommandLineInterface.RegisterCommand({"Get", "Get any variable.",
        LCommandParams()
        .AddToken(LCliType::Type("Var", "The variable to get."))
        .SetExec(LOnCommandInvokation::CreateDelegate([](const LCommandArgs& InArgs, LCommandExecutionResponse* OutResponse) -> void
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
        }))}).IsValid());
    }

    this->ObjectContext.SetHumanReadableName("Engine");
    this->Collection.DeferredInitialize(&this->ObjectContext);
    this->Collection.InitializeSubsystems(JEngineSubsystem::StaticClass());

#if WITH_LOCAL_LAYER
    check( this->LocalEgo.IsValid() == false )
    this->LocalEgo.Initialize();
#endif /* WITH_LOCAL_LAYER */

    this->RegisterLevel(LLevel("LWorld"));

    LWorldContext& Context = this->CreateNewWorldContext("StartUpWorld");
    this->Browse(Context, "LWorld");

    return;
}

void Jafg::LEngine::Tick(const float DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Early, 5);

#if WITH_LOCAL_LAYER
    this->LocalEgo.Tick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

    for (LWorldContext& Context : this->Contexts)
    {
        checkSlow( Context.IsValid() )

        if (Context.IsWaitingForTravel())
        {
            this->TravelContext(Context);
            continue;
        }

        if (Context.ChildWorld->CanTick() == false)
        {
            continue;
        }

        Context.ChildWorld->Tick(DeltaTime);

        continue;
    }

#if WITH_LOCAL_LAYER
    this->LocalEgo.OnLateTick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

#if WITH_LOCAL_LAYER
    for (LWorldContext& Context : this->Contexts)
    {
        checkSlow( Context.IsValid() )

        if (Context.ChildWorld->CanTick())
        {
            Context.ChildWorld->LateTick(DeltaTime);
        }

        continue;
    }
#endif /* WITH_LOCAL_LAYER */

    Tasks::Private::TryRunTasks(ENamedThreads::Master, ETaskTime::Late, 5);

    return;
}

void Jafg::LEngine::TearDown()
{
    STAT_CYCLE_FUNCTION()

    LOG_VERBOSE(LogEngine, "Tearing down engine.")

    LOG_VERBOSE(LogEngine, "Deallocating {} registered contexts.", this->Contexts.GetSize())
    for (LWorldContext& Context : this->Contexts)
    {
        Context.ChildWorld->TearDownContext();
        check( Context.ChildWorld->GetWorldState() == EWorldState::WaitingForKill )
        delete Context.ChildWorld;
        Context.ChildWorld = nullptr;
        continue;
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

Jafg::LWorldContext& Jafg::LEngine::GetContextFromWorld(const LWorld* World)
{
    check( World )

    for (LWorldContext& Context : this->Contexts)
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
    return this->RegisteredLevels.ContainsByPredicate(
        [&Identifier] (const LLevel& i)
        {
            return i.Identifier == Identifier;
        }
    );
}

Jafg::LWorldContext& Jafg::LEngine::CreateNewWorldContext(const LString& InHumanReadableName)
{
    check( Tasks::IsOnMasterThread() )
    this->Contexts.Emplace(InHumanReadableName);
    return *this->Contexts.GetLast();
}

void Jafg::LEngine::Browse(LWorldContext& Context, const LString& Url) const
{
    if (this->IsContextUrlInternal(Url) == false)
    {
        unimplemented()
        return;
    }

    if (this->IsLevelRegistered(Url) == false)
    {
        jassert( false && "Local Url not registered." )
        return;
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

    /* We have to implement this in the future. If not internal, then connect to remote server. */
    return true;
}

void Jafg::LEngine::TravelContext(LWorldContext& Context)
{
    check( Context.IsWaitingForTravel() )

    LLevel* Level = this->GetLevelByInternalUrl(Context.TravelUrl);
    jassert( Level )

    /* Get some args here in the future. */
    Context.TravelUrl.Empty();

    check( Context.ChildWorld )

    if (Context.ChildWorld->GetWorldState() == EWorldState::Running)
    {
        Context.ChildWorld->TearDownContext();
    }

    Context.ChildWorld->InitializeWorld(*Level);

    return;
}
