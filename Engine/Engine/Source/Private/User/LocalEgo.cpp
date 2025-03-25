// Copyright mzoesch. All rights reserved.

#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "Engine/ActorUtility.h"
#include "Framework/Frontend.h"
#include "Framework/Pawn.h"
#include "Framework/PersonaController.h"
#include "Framework/Lackey.h"
#include "MyWorld/MyWorldStatics.h"
#include "Subsystems/LocalEgoSubsystem.h"

void Jafg::LLocalEgo::Initialize()
{
    check( Tasks::IsOnMasterThread() )
    check( this->IsValid() == false )

    this->bValid = true;

    this->Context.SetHumanReadableName("LocalEgo");

    this->OnWorldBeginLifeHandle = GEngine->OnWorldBeginLife.Add(this, &LLocalEgo::OnWorldBeginLife);

    this->Collection.DeferredInitialize(&this->Context);
    this->Collection.InitializeSubsystems(JLocalEgoSubsystem::StaticClass());

    this->Frontend.Initialize(&this->GetContext());

    return;
}

void Jafg::LLocalEgo::Tick(const float DeltaTime)
{
    if (const i32 PurgedFactories = Private::PurgeWidgetFactories(); PurgedFactories > 0)
    {
        LOG_VERBOSE(LogWidgetFramework, "Purged {} widget factories.", PurgedFactories)
    }

    this->Frontend.Tick(&this->UserInput);

    return;
}

void Jafg::LLocalEgo::OnLateTick(const float DeltaTime)
{
    for (LSurface& Surface : this->Frontend.GetSurfaces())
    {
        Surface.OnUpdate();
    }

    return;
}

void Jafg::LLocalEgo::TearDown()
{
    this->Collection.TearDownSubsystems();

    if (ensure(this->OnWorldBeginLifeHandle.IsValid()))
    {
        GEngine->OnWorldBeginLife.Remove(this->OnWorldBeginLifeHandle);
        this->OnWorldBeginLifeHandle.Reset();
    }

    this->Frontend.TearDown();
    this->Context.TearDownContext();
    check( this->Context.IsValid() == false )

    this->bValid = false;

    return;
}

void Jafg::LLocalEgo::Possess(APersonaController* InNewController)
{
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

    this->Collection.ForEachSubsystem<JLocalEgoSubsystem>(
    [Old, InNewController] (JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaControllerPossessed(Old, InNewController);
    });

    return;
}

void Jafg::LLocalEgo::OnNewPawnPossessed(APawn* InOld, APawn* InNew)
{
    if (LSurface* Surface = this->GetFrontend()->GetFocusedSurface(); Surface)
    {
        if (InOld)
        {
            Surface->GetViewport().GetMutableBackgroundContexts().RemoveOnceByPredicate([InOld](const LBackgroundContext& LambdaContext)
            {
                return LambdaContext.Eye == InOld->GetEye();
            });
        }

        if (InNew)
        {
            Surface->GetViewport().GetMutableBackgroundContexts().Emplace(InNew->GetEye(), InNew->GetOrCalculateCastedOuterAsserted());
        }
    }

    this->Collection.ForEachSubsystem<JLocalEgoSubsystem>(
    [InOld, InNew](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPawnPossessed(InOld, InNew);
    });

    return;
}

void Jafg::LLocalEgo::OnWorldBeginLife(LWorld* InNewWorld)
{
    checkSlow( InNewWorld )

    if (this->PersonaController == nullptr)
    {
        APersonaController* Pc = SpawnDeferredActor<APersonaController>(InNewWorld);
        this->Possess(Pc);
    }

    if (this->PersonaController->DoesPossess() == false)
    {
        APawn* Pawn = SpawnDeferredActor<APawn>(InNewWorld, ALackey::StaticClass());
        this->PersonaController->Possess(Pawn);
        Pawn->SetTranslation(LVector(MwStatics::ChunkSize * 0.5f, MwStatics::ChunkSize * 0.5f, 60.0f));
    }

    return;
}
