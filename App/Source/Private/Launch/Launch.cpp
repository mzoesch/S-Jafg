// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Logging/LogPrivate.h"
#include "Private/Engine/CoreGlobals.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>

void RegisterCoreLogCategories()
{
    ensure( REGISTER_LOG_CATEGORY(LogCore)        )
    ensure( REGISTER_LOG_CATEGORY(LogGuardedMain) )

    return;
}

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
    if (GEngine)
    {
        GEngine->TearDown();
        delete GEngine;
        GEngine = nullptr;
    }

    JafgLog::Private::TearDown();

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

    EPlatformExit::Type ErrorLevel = JafgLog::Private::Init();
    if (ErrorLevel != EPlatformExit::Success)
    {
        checkNoEntry()
        return ErrorLevel;
    }
    RegisterCoreLogCategories();

    LOG_DISPLAY(LogGuardedMain, "Hello {}.", "World")

    ErrorLevel = Application::Create();
    if (ErrorLevel != EPlatformExit::Success)
    {
        checkNoEntry()
        return ErrorLevel;
    }

    ErrorLevel = EngineInit();

    if (ErrorLevel != EPlatformExit::Success)
    {
        checkNoEntry()
        return ErrorLevel;
    }

    while (IsTearingDown() == false)
    {
        EngineTick();
    }

    return ErrorLevel;
}
