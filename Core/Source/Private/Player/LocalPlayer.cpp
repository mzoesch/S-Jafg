// Copyright mzoesch. All rights reserved.

#include "Player/LocalPlayer.h"
#include "Platform/Surface.h"
#include "Player/PlayerInput.h"

void JLocalPlayer::Initialize()
{
    check( this->PlayerInput == nullptr )

    this->PlayerInput = new JPlayerInput();

    return;
}

void JLocalPlayer::Tick(const float DeltaTime)
{
    this->PlayerInput->BeginNewFrame();

    this->GetEngine()->GetSurface()->PollInputs();
    this->GetEngine()->GetSurface()->PollEvents();

    this->PlayerInput->ProcessInput();

    return;
}

void JLocalPlayer::TearDown()
{
}
