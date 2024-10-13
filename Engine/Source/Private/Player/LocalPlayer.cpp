// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Player/LocalPlayer.h"
#include "Platform/Surface.h"
#include "Player/PlayerInput.h"

void Jafg::JLocalPlayer::Initialize()
{
    check( this->PlayerInput == nullptr )

    this->PlayerInput = new JPlayerInput();

    return;
}

void Jafg::JLocalPlayer::Tick(const float DeltaTime)
{
    this->PlayerInput->BeginNewFrame();

    this->GetEngine()->GetSurface()->PollInputs();
    this->GetEngine()->GetSurface()->PollEvents();

    this->PlayerInput->ProcessInput();

    return;
}

void Jafg::JLocalPlayer::TearDown()
{
}
