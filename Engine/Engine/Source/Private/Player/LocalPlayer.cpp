// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/LocalPlayer.h"
#include "Engine/Engine.h"
#include "Engine/Framework/Hud.h"
#include "Player/PlayerInput.h"
#include "Platform/Surface.h"
#include "User/UserPreferences.h"
#include "Engine/ActorUtility.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PlayerController.h"
#include "Engine/Framework/Lackey.h"

void Jafg::LLocalPlayer::Initialize()
{
    checkSlow( this->PlayerInput == nullptr )

    this->Context = new ::Jafg::Private::LObjectContext();

    this->PlayerInput = new LPlayerInput();

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

    this->OnWorldBeginLifeHandle = GEngine->OnWorldBeginLife.AddMember(&LLocalPlayer::OnWorldBeginLife, this);

    return;
}

void Jafg::LLocalPlayer::Tick(const float DeltaTime)
{
    this->SurfaceToDrawOn->OnClear();

    this->PlayerInput->BeginNewFrame();
    this->GetPrimarySurface()->PollInputs();
    this->GetPrimarySurface()->PollEvents();
    this->Hud->Tick();
    this->PlayerInput->DispatchInputDelegates();

    return;
}

void Jafg::LLocalPlayer::OnLateTick(const float DeltaTime)
{
    this->GetPrimarySurface()->OnUpdate();

    return;
}

void Jafg::LLocalPlayer::TearDown()
{
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

    if (ensure(this->PlayerInput))
    {
        delete this->PlayerInput;
        this->PlayerInput = nullptr;
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

void Jafg::LLocalPlayer::Possess(APlayerController* InNewController)
{
    this->PlayerController = InNewController;
}

void Jafg::LLocalPlayer::OnWorldBeginLife(LWorld* InNewWorld)
{
    checkSlow( InNewWorld )

    if (this->PlayerController == nullptr)
    {
        APlayerController* Pc = SpawnDeferredActor<APlayerController>(InNewWorld);
        this->Possess(Pc);
    }

    if (this->PlayerController->DoesPossess() == false)
    {
        APawn* Pawn = SpawnDeferredActor<APawn>(InNewWorld, ALackey::StaticClass());
        this->PlayerController->Possess(Pawn);
        Pawn->SetTranslation(LVector(0.0f, 0.0f, 50.0f));
    }

    return;
}
