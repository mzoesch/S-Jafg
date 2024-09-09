// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Core/App.h"
#include "Engine/Engine.h"
#include "Logging/LogPrivate.h"
#include "Runtime/Platform/DesktopPlatform.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/msvc_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include "GLFW/glfw3.h"

void RegisterCoreLogCategories()
{
    ensure( REGISTER_LOG_CATEGORY(LogCore)        )
    ensure( REGISTER_LOG_CATEGORY(LogGuardedMain) )

    return;
}

std::unique_ptr<LPlatformSpecificCallbacks> CreatePlatformSpecificCallbacks()
{
    std::unique_ptr<LPlatformSpecificCallbacks> PlatformSpecificCallbacks = std::make_unique<LPlatformSpecificCallbacks>();

    PlatformSpecificCallbacks->CreateSurface = [] () -> std::unique_ptr<Surface>
    {
#if PLATFORM_DESKTOP
        std::unique_ptr<Surface> DesktopSurface = std::make_unique<DesktopPlatform>();
        DesktopSurface->As<DesktopPlatform>()->Init();
        return DesktopSurface;
#else /* PLATFORM_DESKTOP */
        #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */
    };

    PlatformSpecificCallbacks->OnTearDown = [] (std::unique_ptr<Surface>* ActiveSurface)
    {
#if PLATFORM_DESKTOP
        ActiveSurface->get()->As<DesktopPlatform>()->TearDown();
#else /* PLATFORM_DESKTOP */
    #error "Could not resolve PLATFORM."
#endif /* !PLATFORM_DESKTOP */

        ActiveSurface->reset();

        return;
    };

    PlatformSpecificCallbacks->GetHighestResolutionMonotonicTime = [] () -> double
    {
        return glfwGetTime();
    };

    return PlatformSpecificCallbacks;
}

FORCEINLINE EPlatformExit::Type EngineInit()
{
    if (GEngine)
    {
        return EPlatformExit::Fatal;
    }

    GEngine = new JEngine();

    GEngine->Init(CreatePlatformSpecificCallbacks());

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
