// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "CoreGlobals.h"
#if PLATFORM_DESKTOP
    #include "Platform/DesktopPlatform.h"
#endif /* PLATFORM_DESKTOP */
#include "Core/Application.h"
#include "Engine/World.h"
#include "Player/LocalPlayer.h"

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

namespace Jafg
{

ENGINE_API LEngine*         GEngine                     = nullptr;

ENGINE_API bool             bGShouldRequestExit         = false;
ENGINE_API bool             bGEngineRequestingExit      = false;

ENGINE_API int32            GCustomExitStatusOverride   = INDEX_NONE;
ENGINE_API LStringLegacy          GCustomExitReason           = "";

} /* ~Namespace Jafg. */

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void Jafg::LEngine::Initialize()
{
#if WITH_LOCAL_LAYER
    check( this->LocalPlayer == nullptr )
    this->LocalPlayer = new LLocalPlayer();
    this->LocalPlayer->Initialize();
#endif /* WITH_LOCAL_LAYER */

    LWorldContext& Context = this->CreateNewWorldContext();
    this->InitializeContext(Context);
    this->RegisterLevel(LLevel("LWorld"));
    this->Browse(Context, "LWorld");

    return;
}

void Jafg::LEngine::Tick(const float DeltaTime)
{
#if WITH_LOCAL_LAYER
    this->LocalPlayer->Tick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

    for (LWorldContext* i : this->Contexts)
    {
        if (i == nullptr)
        {
            continue;
        }

        check( i->ChildWorld )

        if (i->IsWaitingForTravel())
        {
            this->TravelContext(*i);
            continue;
        }

        if (i->ChildWorld->CanTick() == false)
        {
            continue;
        }

        i->ChildWorld->Tick(DeltaTime);

        continue;
    }

#if WITH_LOCAL_LAYER
    this->LocalPlayer->OnLateTick(DeltaTime);
#endif /* WITH_LOCAL_LAYER */

    return;
}

void Jafg::LEngine::TearDown()
{
    for (LWorldContext* i : this->Contexts)
    {
        if (i != nullptr)
        {
            if (i->ChildWorld != nullptr)
            {
                i->ChildWorld->TearDownContext();

                check( i->ChildWorld->GetWorldState() == EWorldState::WaitingForKill )

                delete i->ChildWorld;
                i->ChildWorld = nullptr;
            }

            delete i;
            i = nullptr;
        }

        continue;
    }

#if WITH_LOCAL_LAYER
    if (this->LocalPlayer)
    {
        this->LocalPlayer->TearDown();
        delete this->LocalPlayer;
        this->LocalPlayer = nullptr;
    }
#endif /* WITH_LOCAL_LAYER */

    return;
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot update time. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::UpdateTime()
{
    using namespace Jafg;

    Application::SetPreviousFrameTime(Application::GetCurrentFrameTime());
    Application::SetCurrentFrameTime(
        Application::GetTimeDifferenceFromStaticStorageInitialization(
            Application::GetHighestNow()
        )
    );
    Application::SetDeltaTime(Application::GetCurrentFrameTime() - Application::GetPreviousFrameTime());
    if (Application::GetDeltaTime() < Application::GetLowestDeltaTime())
    {
        Application::SetLowestDeltaTime(Application::GetDeltaTime());
    }
    if (Application::GetDeltaTime() > Application::GetHighestDeltaTime())
    {
        Application::SetHighestDeltaTime(Application::GetDeltaTime());
    }

    Application::UpdateFrameCount();

    const Application::Private::LHrcTimePoint CurrentSteadyTime = Application::GetHighestNow();
    if (
            std::chrono::duration<double>(CurrentSteadyTime - Application::GetLastStatisticsTime()).count()
        >
            Application::GetStatisticsPeriod()
    )
    {
        Application::ResetStatistics();
    }

    return;
}

void Jafg::LEngine::EnforceTickRate()
{
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
void Jafg::LEngine::RequestEngineExit(const LStringLegacy& Reason)
{
    ::Jafg::RequestEngineExit(Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus)
{
    ::Jafg::RequestEngineExit(CustomExitStatus);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus, const LStringLegacy& Reason)
{
    ::Jafg::RequestEngineExit(CustomExitStatus, Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot check for rendering state. */
// ReSharper disable once CppMemberFunctionMayBeStatic
bool Jafg::LEngine::CanEverRender() const
{
#if WITH_FRONTEND
    return true;
#else /* WITH_FRONTEND */
    return false;
#endif /* !WITH_FRONTEND */
}

bool Jafg::LEngine::HasPrimarySurface() const
{
    return this->HasLocalPlayer() && this->GetLocalPlayer()->HasPrimarySurface();
}

Jafg::LSurface* Jafg::LEngine::GetPrimarySurface() const
{
    if (this->HasLocalPlayer())
    {
        return this->GetLocalPlayer()->GetPrimarySurface();
    }

    return nullptr;
}

Jafg::LSurface* Jafg::LEngine::GetCheckedPrimarySurface() const
{
    if (LSurface* Surface = this->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    check( false && "Failed to get primary surface." )

    return nullptr;
}

Jafg::LSurface* Jafg::LEngine::GetPanickedPrimarySurface() const
{
    if (LSurface* Surface = this->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    panic( "Failed to get primary surface." )

    return nullptr;
}

uint8 Jafg::LEngine::GetCurrentFreeContexts() const
{
    return LEngine::GetMaxContexts() - this->GetCurrentOccupiedContexts();
}

uint8 Jafg::LEngine::GetCurrentOccupiedContexts() const
{
    uint8 OccupiedContexts = 0;
    for (const LWorldContext* i : this->Contexts)
    {
        if (i != nullptr)
        {
            ++OccupiedContexts;
        }
    }

    return OccupiedContexts;
}

Jafg::LWorldContext& Jafg::LEngine::GetContextFromWorld(const LWorld& World)
{
    for (LWorldContext* i : this->Contexts)
    {
        if (i != nullptr && i->ChildWorld == &World)
        {
            return *i;
        }
    }

    jassertNoEntry()

    return CreateNewWorldContext();
}

void Jafg::LEngine::Browse(LWorldContext& Context, const LStringLegacy& Url) const
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

bool Jafg::LEngine::IsContextUrlInternal(const LStringLegacy& Url) const
{
    if (Url.empty())
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
    if (Level == nullptr)
    {
        jassertNoEntry()
        return;
    }

    /* Get some args here in the future. */
    Context.TravelUrl.clear();

    check( Context.ChildWorld )

    if (Context.ChildWorld->GetWorldState() == EWorldState::Running)
    {
        Context.ChildWorld->TearDownContext();
    }

    Context.ChildWorld->InitializeWorld(*Level);

    return;
}

Jafg::LLevel* Jafg::LEngine::GetLevelByInternalUrl(const LStringLegacy& Url)
{
    return this->RegisteredLevels.FindRef(Url);
}

void Jafg::LEngine::Browse(const LWorld& Context, const LStringLegacy& Url)
{
    this->Browse(this->GetContextFromWorld(Context), Url);
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

bool Jafg::LEngine::RegisterLevel(const LLevel&& InLevel)
{
    if (this->IsLevelRegistered(InLevel.Identifier))
    {
        return false;
    }

    this->RegisteredLevels.Add(InLevel);

    return true;
}

bool Jafg::LEngine::IsLevelRegistered(const LStringLegacy& Identifier) const
{
    return this->RegisteredLevels.ContainsByPredicate(
        [&Identifier] (const LLevel& i)
        {
            return i.Identifier == Identifier;
        }
    );
    //
    // return std::ranges::any_of(
    //     this->RegisteredLevels,
    //     [&Identifier] (const LLevel& i) { return i.Identifier == Identifier; }
    // );
}

uint8 Jafg::LEngine::GetFirstAvailableContextIndex() const
{
    for (uint8 i = 0; i < LEngine::GetMaxContexts(); ++i)
    {
        if (this->Contexts[i] == nullptr)
        {
            return i;
        }
    }

    jassertNoEntry()

    return INDEX_NONE;
}

Jafg::LWorldContext& Jafg::LEngine::CreateNewWorldContext()
{
    if (this->GetCurrentFreeContexts() <= 0)
    {
        jassertNoEntry()
    }

    const uint8 Index = this->GetFirstAvailableContextIndex();
    check( this->Contexts[Index] == nullptr )

    this->Contexts[Index] = new LWorldContext();

    return *this->Contexts[Index];
}

void Jafg::LEngine::InitializeContext(LWorldContext& Context)
{
    jassert( Context.ChildWorld == nullptr )

    Context.ChildWorld = new LWorld(EWorldState::Uninitialized);

    return;
}
