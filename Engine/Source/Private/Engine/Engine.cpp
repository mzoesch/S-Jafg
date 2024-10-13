// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "CoreGlobals.h"
#if PLATFORM_DESKTOP
    #include "Platform/DesktopPlatform.h"
#endif /* PLATFORM_DESKTOP */
#include <glad/glad.h>
#include <glm/gtc/type_ptr.inl>
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
ENGINE_API LString          GCustomExitReason           = "";

}

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void Jafg::LEngine::Init()
{
#if PLATFORM_DESKTOP
    this->SurfaceToDrawOn = new Jafg::DesktopPlatform();
#else /* PLATFORM_DESKTOP */
    #error "Cannot resolve surface.
#endif /* !PLATFORM_DESKTOP */

    this->SurfaceToDrawOn->Initialize();
    this->SurfaceToDrawOn->SetVSync(true);
    this->SurfaceToDrawOn->SetInputMode(false);

    LWorldContext& Context = this->CreateNewWorldContext();
    this->InitializeContext(Context);
    this->RegisterLevel(LLevel("LWorld"));
    this->Browse(Context, "LWorld");

    check( this->LocalPlayer == nullptr )
    this->LocalPlayer = new JLocalPlayer();
    this->LocalPlayer->Initialize();

    return;
}

void Jafg::LEngine::Tick(const float DeltaTime)
{
    this->SurfaceToDrawOn->OnClear();
    this->LocalPlayer->Tick(DeltaTime);

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

    this->SurfaceToDrawOn->OnUpdate();

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
                i->ChildWorld->TearDownWorld();

                check( i->ChildWorld->GetWorldState() == EWorldState::WaitingForKill )

                delete i->ChildWorld;
                i->ChildWorld = nullptr;
            }

            delete i;
            i = nullptr;
        }

        continue;
    }

    if (this->SurfaceToDrawOn)
    {
        this->SurfaceToDrawOn->TearDown();
        delete this->SurfaceToDrawOn;
        this->SurfaceToDrawOn = nullptr;
    }

    return;
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot update time. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::UpdateTime()
{
    using namespace Jafg;

    Application::SetPreviousFrameTime(Application::GetCurrentFrameTime());
    // Application::SetCurrentFrameTime(glfwGetTime());

    Application::SetDeltaTime(Application::GetCurrentFrameTime() - Application::GetPreviousFrameTime());

    Application::SetCurrentFps(1.0f / Application::GetDeltaTimeAsFloat());
    if (Application::GetLowestFps() < 0.0f || Application::GetCurrentFps() < Application::GetLowestFps())
    {
        Application::SetLowestFps(Application::GetCurrentFps());
    }
    if (Application::GetHighestFps() < 0.0f || Application::GetCurrentFps() > Application::GetHighestFps())
    {
        Application::SetHighestFps(Application::GetCurrentFps());
    }

    Application::UpdateFrameCount();

    const SteadyStatisticsTimePoint CurrentSteadyTime = std::chrono::steady_clock::now();
    if (
        const int64 StatsTimeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(CurrentSteadyTime - Application::GetLastStatisticsTime()).count();
        StatsTimeDelta > 10'000
    )
    {
        // std::cout << "Time Stats: Avg: " << static_cast<float>(Application::GetStatisticsFrameCount()) /
        //     10'000.0f * 1'000.0f << ", Low: " << Application::GetLowestFps() << ", High: " << Application::GetHighestFps() <<
        //     '\n';
        // std::cout.flush();

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
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus)
{
    ::Jafg::RequestEngineExit(CustomExitStatus);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus, const LString& Reason)
{
    ::Jafg::RequestEngineExit(CustomExitStatus, Reason);
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

Jafg::JWorld* Jafg::LEngine::GetFirstRunningWorld()
{
    for (const LWorldContext* i : this->Contexts)
    {
        if (i != nullptr && i->ChildWorld != nullptr && i->ChildWorld->GetWorldState() == EWorldState::Running)
        {
            return i->ChildWorld;
        }

        continue;
    }

    return nullptr;
}

Jafg::LWorldContext& Jafg::LEngine::GetContextFromWorld(const JWorld& World)
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
        Context.ChildWorld->TearDownWorld();
    }

    Context.ChildWorld->InitializeWorld(*Level);

    return;
}

Jafg::LLevel* Jafg::LEngine::GetLevelByInternalUrl(const LString& Url)
{
    for (LLevel& i : this->RegisteredLevels)
    {
        if (i.Identifier == Url)
        {
            return &i;
        }
    }

    return nullptr;
}

void Jafg::LEngine::Browse(const JWorld& Context, const LString& Url)
{
    this->Browse(this->GetContextFromWorld(Context), Url);
}

bool Jafg::LEngine::RegisterLevel(LLevel InLevel)
{
    if (this->IsLevelRegistered(InLevel.Identifier))
    {
        return false;
    }

    this->RegisteredLevels.emplace_back(std::move(InLevel));

    return true;
}

bool Jafg::LEngine::IsLevelRegistered(const LString& Identifier) const
{
    return std::ranges::any_of(
        this->RegisteredLevels,
        [&Identifier] (const LLevel& i) { return i.Identifier == Identifier; }
    );
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

    Context.ChildWorld = new JWorld(EWorldState::Uninitialized);

    return;
}
