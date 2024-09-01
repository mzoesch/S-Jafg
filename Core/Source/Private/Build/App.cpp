// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/App.h"

class App;

App* GApp = nullptr;

class App
{
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

float Application::GetDeltaTime()
{
    return 0.1f;
}
