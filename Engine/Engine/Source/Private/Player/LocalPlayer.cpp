// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/LocalPlayer.h"
#include "Player/PlayerInput.h"
#include "Platform/Surface.h"

void Jafg::LLocalPlayer::Initialize()
{
    checkSlow( this->PlayerInput == nullptr )

    this->PlayerInput = new LPlayerInput();

#if PLATFORM_DESKTOP
    this->SurfaceToDrawOn = new ::Jafg::LDesktopPlatform();
#else /* PLATFORM_DESKTOP */
    #error "Cannot resolve surface.
#endif /* !PLATFORM_DESKTOP */

    this->GetPrimarySurface()->Initialize();
    this->GetPrimarySurface()->SetVSync(true);
    this->GetPrimarySurface()->SetInputMode(false);

    return;
}

void Jafg::LLocalPlayer::Tick(const float DeltaTime)
{
    this->SurfaceToDrawOn->OnClear();

    this->PlayerInput->BeginNewFrame();
    this->GetPrimarySurface()->PollInputs();
    this->GetPrimarySurface()->PollEvents();
    this->PlayerInput->DispatchInputDelegates();

    return;
}

void Jafg::LLocalPlayer::OnLateTick(const float DeltaTime)
{
    this->GetPrimarySurface()->OnUpdate();
}

void Jafg::LLocalPlayer::TearDown()
{
    if (this->PlayerInput)
    {
        delete this->PlayerInput;
        this->PlayerInput = nullptr;
    }

    if (this->SurfaceToDrawOn)
    {
        this->SurfaceToDrawOn->TearDown();
        delete this->SurfaceToDrawOn;
        this->SurfaceToDrawOn = nullptr;
    }

    return;
}

void Jafg::LLocalPlayer::Possess(APlayerController* InNewController)
{
    this->PlayerController = InNewController;
}
