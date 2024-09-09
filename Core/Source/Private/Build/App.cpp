// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/App.h"

struct App;

App* GApp = nullptr;

struct App
{
    double DeltaTime  = 0.0f;
    uint64 FrameCount = 0;
};

EPlatformExit::Type Application::Create()
{
    if (GApp)
    {
        return EPlatformExit::Fatal;
    }

    GApp = new App();

    return EPlatformExit::Success;
}

void Application::SetDeltaTime(const double DeltaTime)
{
    GApp->DeltaTime = DeltaTime;
}

double Application::GetDeltaTime()
{
    return GApp->DeltaTime;
}

float Application::GetDeltaTimeAsFloat()
{
    return static_cast<float>(GApp->DeltaTime);
}

void Application::UpdateFrameCount()
{
    GApp->FrameCount++;
}

uint64 Application::GetFrameCount()
{
    return GApp->FrameCount;
}
