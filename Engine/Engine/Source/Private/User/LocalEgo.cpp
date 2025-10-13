// Copyright mzoesch. All rights reserved.

#include "User/LocalEgo.h"
#include "Core/CoreNames.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Framework/Lackey.h"
#include "MyWorld/MyWorldStatics.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Stats/Stats.h"

void Jafg::LLocalEgo::Initialize()
{
    STAT_CYCLE_FUNCTION()

    check( Tasks::IsOnMasterThread() )
    check( this->IsValid() == false )
    this->bValid = true;

    LCommandLineInterface* Cli = GEngine->GetCommandLineInterface();
    this->VariableHandle_UpdateFrustum = Cli->RegisterVariable({"UpdateFrustum", LCliType::Type("Bool"), "true"});
    this->VariableHandle_VisualizeFrustum = Cli->RegisterVariable({"VisualizeFrustum", LCliType::Type("Bool"), "false"});
    this->VariableHandle_FrustumNearPlane = Cli->RegisterVariable({"NearFrustumPlane", LCliType::Type("Float"), "0.1f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if
        (
               GEngine
            && GEngine->GetLocalEgo()
            && GEngine->GetLocalEgo()->GetPossessed()
            && GEngine->GetLocalEgo()->GetPossessed()->GetPossessed()
        )
        {
            f32 NearFrustum; Serialization::FromString(&NearFrustum, InValue);
            GEngine->GetLocalEgo()->GetPossessed()->GetPossessed()->GetEye()->SetNearFrustum(NearFrustum);
            LOG_VERBOSE(LogEgo, "Set current possessed eye near frustum to [{}].", NearFrustum)
        }
        return;
    })});
    this->VariableHandle_FrustumFarPlane = Cli->RegisterVariable({"FarFrustumPlane", LCliType::Type("Float"), "2000.0f",
    LOnVariableChangedDelegate::CreateStrong([](const LString& InValue) -> void
    {
        if
        (
               GEngine
            && GEngine->GetLocalEgo()
            && GEngine->GetLocalEgo()->GetPossessed()
            && GEngine->GetLocalEgo()->GetPossessed()->GetPossessed()
        )
        {
            f32 FarFrustum; Serialization::FromString(&FarFrustum, InValue);
            GEngine->GetLocalEgo()->GetPossessed()->GetPossessed()->GetEye()->SetFarFrustum(FarFrustum);
            LOG_VERBOSE(LogEgo, "Set current possessed eye far frustum to [{}].", FarFrustum)
        }
        return;
    })});
    this->VariableHandle_VerifyChunks = Cli->RegisterVariable({"VerifyChunks", LCliType::Type("Bool"), "true"});

    this->OnWorldBeginLifeHandle = GEngine->OnWorldBeginLife.Add(this, &LLocalEgo::OnWorldBeginLife);

    this->Collection.InitializeDeferred(&this->Outer);
    this->Collection.InitializeSubsystems<JLocalEgoSubsystem>();

    this->Frontend.Initialize(&this->Outer);

    return;
}

void Jafg::LLocalEgo::Tick(const float DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    if (const i32 PurgedFactories = Private::PurgeWidgetFactories(); PurgedFactories > 0)
    {
        LOG_TRACE(LogWidgetFramework, "Purged {} widget factories.", PurgedFactories)
    }

    this->Frontend.Tick(&this->UserInput);

    return;
}

void Jafg::LLocalEgo::OnLateTick(const float DeltaTime)
{
    STAT_CYCLE_FUNCTION()

    for (LSurface& Surface : this->Frontend.GetSurfaces())
    {
        Surface.OnUpdate();
    }

    return;
}

void Jafg::LLocalEgo::TearDown()
{
    if (ensure(this->bValid) == false)
    {
        return;
    }

    check( GEngine )

    this->Collection.TearDownSubsystems();

    if (ensure(this->OnWorldBeginLifeHandle.IsValid()))
    {
        GEngine->OnWorldBeginLife.Remove(&this->OnWorldBeginLifeHandle);
        this->OnWorldBeginLifeHandle.Reset();
    }

    this->Frontend.TearDown();
    this->Outer.TearDown();

    GEngine->GetCommandLineInterface()->UnregisterVariable(&this->VariableHandle_UpdateFrustum);
    GEngine->GetCommandLineInterface()->UnregisterVariable(&this->VariableHandle_VisualizeFrustum);
    GEngine->GetCommandLineInterface()->UnregisterVariable(&this->VariableHandle_FrustumNearPlane);
    GEngine->GetCommandLineInterface()->UnregisterVariable(&this->VariableHandle_FrustumFarPlane);
    GEngine->GetCommandLineInterface()->UnregisterVariable(&this->VariableHandle_VerifyChunks);

    this->bValid = false;
    return;
}

void Jafg::LLocalEgo::Possess(APersonaController* InNewController)
{
    if (InNewController)
    {
        this->GetUserInput()->ActivateContext(Name_UicInMyWorld);
    }
    else
    {
        this->GetUserInput()->DeactivateContext(Name_UicInMyWorld);
    }

    APersonaController* Old = this->PersonaController;
    this->PersonaController = InNewController;

    if (Old)
    {
        Old->SetLocalEgo(nullptr);
    }
    if (InNewController)
    {
        InNewController->SetLocalEgo(this);
    }

    this->ForEachMutableSubsystem([Old, InNewController](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaControllerPossessed(Old, InNewController);
    });

    return;
}

void Jafg::LLocalEgo::OnNewPawnPossessed(APawn* InOld, APawn* InNew)
{
    if (InNew)
    {
        this->UserInput.ActivateContext(Name_UicInMyWorldFoot);
    }
    else
    {
        this->UserInput.DeactivateContext(Name_UicInMyWorldFoot);
    }

    if (LSurface* Surface = this->GetFrontend()->GetFocusedSurface(); Surface)
    {
        if (InOld)
        {
            algo::erase_once_checked(&Surface->GetViewport().GetMutableBackgroundContexts(), InOld->GetEye(), &LBackgroundContext::Eye);
        }

        if (InNew)
        {
            Surface->GetViewport().GetMutableBackgroundContexts().emplace_back(InNew->GetEye(), InNew->GetOrCalculateCastedOuterAsserted());
        }
    }

    this->ForEachMutableSubsystem([InOld, InNew](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPawnPossessed(InOld, InNew);
    });

    return;
}

Jafg::LEngine* Jafg::LLocalEgo::GetEngine()
{
    return GEngine;
}

Jafg::LCommandLineInterface* Jafg::LLocalEgo::GetCommandLineInterface()
{
    return GEngine->GetCommandLineInterface();
}

void Jafg::LLocalEgo::OnWorldBeginLife(LWorld* InNewWorld)
{
    check( Tasks::IsOnMasterThread() )
    checkSlow( InNewWorld )

    if (this->PersonaController == nullptr)
    {
        this->UserInput.DeactivateAllContexts();

        APersonaController* Pc = SpawnDeferredActor<APersonaController>(InNewWorld);
        this->Possess(Pc);

        if (LSurface* Surface = this->GetFrontend()->GetFocusedSurface(); Surface)
        {
            Surface->SetInputMode(InNewWorld->GetUnderlyingLevel().InputMode, InNewWorld->GetUnderlyingLevel().bShowMouseCursor);
            Surface->GetViewport().SetBackgroundColor(InNewWorld->GetUnderlyingLevel().BackgroundColor);
        }

        if (InNewWorld->GetUnderlyingLevelName() == Name_LevelMyWorld.ToString())
        {
            if (this->PersonaController->DoesPossess() == false)
            {
                APawn* Pawn = SpawnDeferredActor<APawn>(InNewWorld, ALackey::StaticClass());
                this->PersonaController->Possess(Pawn);
                Pawn->SetTranslation(LVector(MwStatics::ChunkSize * 0.5f, MwStatics::ChunkSize * 0.5f, MwStatics::ChunkSize * 4.0f + MwStatics::ChunkSize / 2.0f));
            }
        }
    }

    return;
}
