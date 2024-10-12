// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "CoreGlobals.h"
#include "Core/Application.h"

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

namespace Jafg
{

ENGINE_API LEngine*  GEngine                     = nullptr;

ENGINE_API bool            bGShouldRequestExit         = false;
bool            bGEngineRequestingExit      = false;

int32           GCustomExitStatusOverride   = INDEX_NONE;
LString         GCustomExitReason           = "";

}

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void Jafg::LEngine::Init()
{
}

void Jafg::LEngine::Tick(const float DeltaTime)
{
}

void Jafg::LEngine::TearDown()
{
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
    Jafg::Private::BeginExitIfRequested();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit()
{
    Jafg::RequestEngineExit();
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const LString& Reason)
{
    Jafg::RequestEngineExit(Reason);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus)
{
    Jafg::RequestEngineExit(CustomExitStatus);
}

/* It does not really make sense to make this static, as if there is no global engine object we cannot exit. */
// ReSharper disable once CppMemberFunctionMayBeStatic
void Jafg::LEngine::RequestEngineExit(const int32 CustomExitStatus, const LString& Reason)
{
    Jafg::RequestEngineExit(CustomExitStatus, Reason);
}
