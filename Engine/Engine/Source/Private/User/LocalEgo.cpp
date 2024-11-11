// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/LocalEgo.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Hud.h"
#include "User/Input/UserInput.h"
#include "Platform/Surface.h"
#include "User/UserPreferences.h"
#include "Engine/ActorUtility.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "Engine/Framework/Lackey.h"
#include "MyWorld/MyWorldStatics.h"
#include "Subsystems/LocalEgoSubsystem.h"
#include "Subsystems/SubsystemCollection.h"

void Jafg::LLocalEgo::Initialize()
{
    checkSlow( this->UserInput == nullptr )

    this->Context = new ::Jafg::Private::LObjectContext();
    this->Context->SetHumanReadableName("LocalEgo");

    this->UserInput = new LUserInput();

#if PLATFORM_DESKTOP
    this->SurfaceToDrawOn = new ::Jafg::LDesktopPlatform();
#else /* PLATFORM_DESKTOP */
    #error "Cannot resolve surface.
#endif /* !PLATFORM_DESKTOP */

    JUserPreferences* UserPreferences = GetMutableDefault<JUserPreferences>();
    UserPreferences->SetVSyncEnabled(true);

    this->GetPrimarySurface()->Initialize();
    this->GetPrimarySurface()->SetVSync(UserPreferences->GetVSyncEnabled());
    this->GetPrimarySurface()->SetInputMode(false);

    this->Hud = new ::Jafg::LHud();
    this->Hud->Initialize(this->GetContext());

    this->OnWorldBeginLifeHandle = GEngine->OnWorldBeginLife.AddMember(&LLocalEgo::OnWorldBeginLife, this);

    checkSlow( this->Collection == nullptr )
    this->Collection = new LSubsystemCollection(this->Context);
    this->Collection->LocateAllSubsystemsOfClass(JLocalEgoSubsystem::StaticClass());
    this->Collection->InitializeSubsystems();

    return;
}

void Jafg::LLocalEgo::Tick(const float DeltaTime)
{
    this->SurfaceToDrawOn->OnClear();

    this->UserInput->BeginNewFrame();
    this->GetPrimarySurface()->PollInputs();
    this->GetPrimarySurface()->PollEvents();
    this->Hud->Tick();
    this->UserInput->DispatchInputDelegates();

    return;
}

void Jafg::LLocalEgo::OnLateTick(const float DeltaTime)
{
    this->GetPrimarySurface()->OnUpdate();

    return;
}

void Jafg::LLocalEgo::TearDown()
{
    checkSlow( this->Collection )
    this->Collection->TearDownSubsystems();
    delete this->Collection;
    this->Collection = nullptr;

    if (ensure(this->OnWorldBeginLifeHandle.IsValid()))
    {
        GEngine->OnWorldBeginLife.Remove(this->OnWorldBeginLifeHandle);
        this->OnWorldBeginLifeHandle.Reset();
    }

    if (ensure(this->Hud))
    {
        this->Hud->TearDown();
        delete this->Hud;
        this->Hud = nullptr;
    }

    if (ensure(this->UserInput))
    {
        delete this->UserInput;
        this->UserInput = nullptr;
    }

    if (ensure(this->SurfaceToDrawOn))
    {
        this->SurfaceToDrawOn->TearDown();
        delete this->SurfaceToDrawOn;
        this->SurfaceToDrawOn = nullptr;
    }

    check( this->Context )
    this->Context->TearDownContext();
    delete this->Context;
    this->Context = nullptr;

    return;
}

void Jafg::LLocalEgo::Possess(APersonaController* InNewController)
{
    APersonaController* Old = this->PersonaController;
    this->PersonaController = InNewController;

    this->Collection->ForEachSubsystem<JLocalEgoSubsystem>(
    [Old, InNewController] (JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaControllerPossessed(Old, InNewController);
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
        Pawn->SetTranslation(LVector(MwStatics::ChunkSize * 0.5f, MwStatics::ChunkSize * 0.5f, 25.0f));
    }

    return;
}
