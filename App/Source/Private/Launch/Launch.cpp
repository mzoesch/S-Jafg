// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Private/Engine/CoreGlobals.h"

FORCEINLINE EPlatformExit::Type EngineInit()
{
    if (GEngine)
    {
        return EPlatformExit::Fatal;
    }

    GEngine = new Engine();

    GEngine->Init();

    return EPlatformExit::Success;
}

FORCEINLINE void EngineTick()
{
    BeginExitIfRequested();

    {
        GEngine->UpdateTime();
        GEngine->EnforceTickRate();
    }

    GEngine->Tick(Application::GetDeltaTime());

    return;
}

FORCEINLINE void EngineExit()
{
    GEngine->TearDown();
    delete GEngine;
    GEngine = nullptr;

    return;
}

EPlatformExit::Type GuardedMain(const TChar* CmdLine)
{
    struct GuardedMainScope
    {
        ~GuardedMainScope()
        {
            EngineExit();
        }
    } GuardedMainScope;

    EPlatformExit::Type ErrorLevel = Application::Create();

    if (ErrorLevel != EPlatformExit::Success)
    {
        return ErrorLevel;
    }

    ErrorLevel = EngineInit();

    if (ErrorLevel != EPlatformExit::Success)
    {
        return ErrorLevel;
    }

    while (IsTearingDown() == false)
    {
        EngineTick();
    }

    return ErrorLevel;
}
