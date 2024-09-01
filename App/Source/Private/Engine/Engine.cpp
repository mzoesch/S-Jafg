// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "IO/DesktopPlatform.h"

///////////////////////////////////////////////////////////////////////////////
// Engine Globals

Engine* GEngine                     = nullptr;

bool    bGShouldRequestExit         = false;
bool    bGEngineRequestingExit      = false;

int32   GCustomExitStatusOverride   = INDEX_NONE;
String  GCustomExitReason           = "";

// ~Engine Globals
///////////////////////////////////////////////////////////////////////////////

void Engine::Init()
{
#if PLATFORM_DESKTOP
    this->ActiveSurface = std::make_unique<DesktopPlatform>();
    this->ActiveSurface->As<DesktopPlatform>()->Init();
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */

    return;
}

void Engine::Tick(const float DeltaSeconds)
{
    if (this->ActiveSurface)
    {
        this->ActiveSurface->OnUpdate();
    }

    return;
}

void Engine::TearDown()
{
#if PLATFORM_DESKTOP
    this->ActiveSurface->As<DesktopPlatform>()->TearDown();
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */

    this->ActiveSurface = nullptr;

    return;
}

void Engine::UpdateTime()
{
}

void Engine::EnforceTickRate()
{
}
