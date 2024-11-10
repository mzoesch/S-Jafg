// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Engine/Engine.h"
#include "Core/Application.h"
#include "Engine/ObjectBaseUtility.h"
#include "Engine/Carnifex.h"

using namespace Jafg;

namespace
{
    LCarnifex* PrivateCarnifex = nullptr;
}

FORCEINLINE EPlatformExit::Type GetMostSignificantExitReason()
{
    /*
     * Even if the engine is null, this will not cause a crash as this member method must always behave in a static
     * way if itself is not defined.
     */
    GEngine->ReflectForwardedExitRequest();

    return ::HasCustomExitStatus()
        ? static_cast<EPlatformExit::Type>(::GetCustomExitStatus())
        : EPlatformExit::Success;
}

FORCEINLINE EPlatformExit::Type EngineInit()
{
    PrivateCarnifex            = new LCarnifex();
    Private::GCarnifexReferrer = &PrivateCarnifex;
    GOmniVitaContext           = new Private::LObjectContext();
    GOmniVitaContext->SetHumanReadableName("OmniVitaContext");
    check( GOmniVitaContext->GetCarnifex() )

    Private::CreateSingletonObjectRegistry();
    Private::GObjectRegistry->LoadPendingPackages();

    if (GEngine)
    {
        return EPlatformExit::Fatal;
    }

    GEngine = new LEngine();

    GEngine->Initialize();

    if (::IsEngineExitRequested())
    {
        ::GetMostSignificantExitReason();
    }

    return EPlatformExit::Success;
}

FORCEINLINE void EngineTick()
{
    LOG_PRIVATE_UNSAFE_FLUSH_EVERYTHING_FAST() /* Just temporary. */

    GEngine->BeginExitIfRequested();

    {
        GEngine->UpdateTime();
        GEngine->EnforceTickRate();
    }

    GEngine->Tick(Application::GetDeltaTimeAsFloat());

    PrivateCarnifex->KillAllGarbageChildren();

    return;
}

FORCEINLINE void EngineExit()
{
    LOG_INFO(LogGuardedMain, "Engine is exiting ...")

    if (GEngine)
    {
        GEngine->TearDown();
        delete GEngine;
        GEngine = nullptr;
    }
    /*
     * Something very eccentric has happened or the pre-life-engine tasks failed to initialize before
     * the engine was even created.
     */
    else
    {
        /*
         * Even if the engine is null, this will not cause a crash as this member method must always behave in a static
         * way if itself is not defined.
         */
        GEngine->ReflectForwardedExitRequest();
    }

    if (ensure(GOmniVitaContext))
    {
        GOmniVitaContext->TearDownContext();
        delete GOmniVitaContext;
        GOmniVitaContext = nullptr;
    }

    PrivateCarnifex->KillAllGarbageChildren();
    delete PrivateCarnifex;
    ::Jafg::Private::GCarnifexReferrer = nullptr;
    PrivateCarnifex = nullptr;

    if (::HasCustomExitReason())
    {
        LOG_INFO(LogGuardedMain, "Engine exit with custom exit reason: {}", ::GetCustomExitReason())
    }

    if (::HasCustomExitStatus())
    {
        LOG_INFO(LogGuardedMain, "Engine exit with custom exit status: {}", ::GetCustomExitStatus())
    }

    Private::KillSingletonObjectRegistry();

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

    LOG_INFO(LogGuardedMain, "Finished static storage initialization after {} seconds.",
        Application::GetDeltaSinceStaticStorageInitialization())

    const EPlatformExit::Type ErrorLevel = EngineInit();
    if (ErrorLevel != EPlatformExit::Success)
    {
        return ErrorLevel;
    }

    while (::IsTearingDown() == false)
    {
        EngineTick();
    }

    return GetMostSignificantExitReason();
}
