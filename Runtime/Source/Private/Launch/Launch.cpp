// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "Core/Application.h"

using namespace Jafg;

FORCEINLINE EPlatformExit::Type EngineInit()
{
    if (GEngine)
    {
        return EPlatformExit::Fatal;
    }

    GEngine = new LEngine();

    GEngine->Init();

    if (::IsEngineExitRequested())
    {
        return EPlatformExit::Fatal;
    }

    return EPlatformExit::Success;
}

FORCEINLINE void EngineTick()
{
    GEngine->BeginExitIfRequested();

    {
        GEngine->UpdateTime();
        GEngine->EnforceTickRate();
    }

    GEngine->Tick(Application::GetDeltaTimeAsFloat());

    return;
}

FORCEINLINE void EngineExit()
{
    std::cout << "Engine exit." << '\n';

    if (GEngine)
    {
        GEngine->TearDown();
        delete GEngine;
        GEngine = nullptr;
    }

    if (::HasCustomExitReason())
    {
        std::cout << "Engine exit requested: " << ::GetCustomExitReason() << '\n';
    }

    if (::HasCustomExitStatus())
    {
        std::cout << "Engine exit status: " << ::GetCustomExitStatus() << '\n';
    }

    return;
}

EPlatformExit::Type GuardedMain(const LChar* CmdLine)
{
    struct GuardedMainScope
    {
        ~GuardedMainScope()
        {
            EngineExit();
        }
    } GuardedMainScope;

    EPlatformExit::Type ErrorLevel = EngineInit();

    if (ErrorLevel != EPlatformExit::Success)
    {
        return ErrorLevel;
    }

    while (::IsTearingDown() == false)
    {
        EngineTick();
    }

    return ErrorLevel;
}
