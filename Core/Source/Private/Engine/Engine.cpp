// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "Engine/WorldContext.h"
#include "CoreGlobals.h"
#include "Core/App.h"
#include "Engine/World.h"

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

JEngine* GEngine                     = nullptr;

bool    bGShouldRequestExit         = false;
bool    bGEngineRequestingExit      = false;

int32   GCustomExitStatusOverride   = INDEX_NONE;
String  GCustomExitReason           = "";

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void JEngine::Init(std::unique_ptr<LPlatformSpecificCallbacks>&& InPlatformSpecificCallbacks)
{
    check( this->Contexts.empty() )

    this->PlatformSpecificCallbacks = std::move(InPlatformSpecificCallbacks);

    LWorldContext& Context = this->CreateNewWorldContext();
    this->Browse(Context, "LWorld");

    this->ActiveSurface = this->PlatformSpecificCallbacks->CreateSurface();

    return;
}

void JEngine::Tick(const float DeltaSeconds)
{
    for (LWorldContext* Context : this->Contexts)
    {
        if (Context->TravelUrl.empty() == false)
        {
            this->TickForWorldTravel(*Context);
        }

        continue;
    }


    if (this->ActiveSurface)
    {
        this->ActiveSurface->OnUpdate();
    }

    return;
}

void JEngine::TearDown()
{
    check( this->PlatformSpecificCallbacks )

    this->PlatformSpecificCallbacks->OnTearDown(&this->ActiveSurface);

    check( this->ActiveSurface == nullptr )

    return;
}

void JEngine::UpdateTime()
{
    this->LastFrameTime    = this->CurrentFrameTime;
    this->CurrentFrameTime = this->PlatformSpecificCallbacks->GetHighestResolutionMonotonicTime();

    const double DeltaTime = this->CurrentFrameTime - this->LastFrameTime;

    Application::SetDeltaTime(DeltaTime);

    this->CurrentFps = 1.0f / static_cast<float>(DeltaTime);
    if (this->LowestFps < 0.0f || this->CurrentFps < this->LowestFps)
    {
        this->LowestFps = this->CurrentFps;
    }
    if (this->HighestFps < 0.0f || this->CurrentFps > this->HighestFps)
    {
        this->HighestFps = this->CurrentFps;
    }

    Application::UpdateFrameCount();
    ++this->StatisticsFrameCount;

    const SteadyStatisticsTimePoint CurrentSteadyTime = std::chrono::steady_clock::now();
    if (
        const int64 StatsTimeDelta = std::chrono::duration_cast<std::chrono::milliseconds>(CurrentSteadyTime - this->StatisticsSteadyTimeStart).count();
        StatsTimeDelta > 10'000
    )
    {
        LOG_DEBUG(LogCore, "Time Stats: Avg: {}, Low: {:.2f}, High: {:.2f}", static_cast<float>(this->StatisticsFrameCount) / 10'000.0f * 1'000.0f, this->LowestFps, this->HighestFps)

        this->StatisticsSteadyTimeStart = CurrentSteadyTime;
        this->StatisticsFrameCount      = 0;
        this->LowestFps                 = -1.0f;
        this->HighestFps                = -1.0f;
    }

    return;
}

void JEngine::EnforceTickRate()
{
}

void JEngine::Browse(LWorldContext& Context, const String& Url)
{
    Context.TravelUrl = Url;

    return;
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

void JEngine::TickForWorldTravel(LWorldContext& Context)
{
    check( Context.TravelUrl.empty() == false )

    if (Context.ChildWorld)
    {
        Context.ChildWorld->TearDown();
        delete Context.ChildWorld;
    }

    Context.ChildWorld = new JWorld { };

    this->InitializeContext(Context);

    return;
}

void JEngine::InitializeContext(LWorldContext& Context)
{
    Context.ChildWorld->Init();

    return;
}

LWorldContext& JEngine::CreateNewWorldContext()
{
    LWorldContext* Context = new LWorldContext { };
    this->Contexts.emplace_back(Context);
    return *Context;
}
