// Copyright mzoesch. All rights reserved.

#include "Engine/Engine.h"
#include "Engine/Framework/Camera.h"
#include "Engine/CoreGlobals.h"
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

JEngine* GEngine                     = nullptr;

bool     bGShouldRequestExit         = false;
bool     bGEngineRequestingExit      = false;

int32    GCustomExitStatusOverride   = INDEX_NONE;
String   GCustomExitReason           = "";

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void FramebufferSizeCallback(GLFWwindow* Window, const int32 Width, const int32 Height)
{
    glViewport(0, 0, Width, Height);
}

void JEngine::Init()
{
#if PLATFORM_DESKTOP
    this->SurfaceToDrawOn = new DesktopPlatform();
#else /* PLATFORM_DESKTOP */
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

void JEngine::Tick(const float DeltaTime)
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

void JEngine::TearDown()
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
void JEngine::UpdateTime()
{
    Application::SetPreviousFrameTime(Application::GetCurrentFrameTime());
    Application::SetCurrentFrameTime(glfwGetTime());

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

void JEngine::EnforceTickRate()
{
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void JEngine::BeginExitIfRequested()
{
    ::BeginExitIfRequested();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void JEngine::RequestEngineExit()
{
    ::RequestEngineExit();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void JEngine::RequestEngineExit(const String& Reason)
{
    ::RequestEngineExit(Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void JEngine::RequestEngineExit(const int32 CustomExitStatus)
{
    ::RequestEngineExit(CustomExitStatus);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void JEngine::RequestEngineExit(const int32 CustomExitStatus, const String& Reason)
{
    ::RequestEngineExit(CustomExitStatus, Reason);
}

uint8 JEngine::GetCurrentFreeContexts() const
{
    return JEngine::GetMaxContexts() - this->GetCurrentOccupiedContexts();
}

uint8 JEngine::GetCurrentOccupiedContexts() const
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

JWorld* JEngine::GetFirstRunningWorld()
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

LWorldContext& JEngine::GetContextFromWorld(const JWorld& World)
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

void JEngine::Browse(LWorldContext& Context, const String& Url) const
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

bool JEngine::IsContextUrlInternal(const String& Url) const
{
    if (Url.empty())
    {
        return false;
    }

    /* We have to implement this in the future. If not internal, then connect to remote server. */
    return true;
}

void JEngine::TravelContext(LWorldContext& Context)
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

LLevel* JEngine::GetLevelByInternalUrl(const String& Url)
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

void JEngine::Browse(const JWorld& Context, const String& Url)
{
    this->Browse(this->GetContextFromWorld(Context), Url);
}

bool JEngine::RegisterLevel(LLevel InLevel)
{
    if (this->IsLevelRegistered(InLevel.Identifier))
    {
        return false;
    }

    this->RegisteredLevels.emplace_back(std::move(InLevel));

    return true;
}

bool JEngine::IsLevelRegistered(const String& Identifier) const
{
    return std::ranges::any_of(
        this->RegisteredLevels,
        [&Identifier] (const LLevel& i) { return i.Identifier == Identifier; }
    );
}

uint8 JEngine::GetFirstAvailableContextIndex() const
{
    for (uint8 i = 0; i < JEngine::GetMaxContexts(); ++i)
    {
        if (this->Contexts[i] == nullptr)
        {
            return i;
        }
    }

    jassertNoEntry()

    return INDEX_NONE;
}

LWorldContext& JEngine::CreateNewWorldContext()
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

void JEngine::InitializeContext(LWorldContext& Context)
{
    jassert( Context.ChildWorld == nullptr )

    Context.ChildWorld = new JWorld(EWorldState::Uninitialized);

    return;
}
