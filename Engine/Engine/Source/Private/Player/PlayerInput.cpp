// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include <intrin.h>
#include "Player/PlayerInput.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Framework/Camera.h"
#include "Engine/Framework/PlayerController.h"
#include "Platform/Surface.h"
#include "Player/LocalPlayer.h"

void Jafg::LPlayerInput::BeginNewFrame()
{
    LSurface* Context = this->GetCheckedPrimaryContext();

    Context->GetCurrentlyPressedKeys().SwapBuffers(Context->GetLastFramePressedKeys());
    Context->GetCurrentlyPressedKeys().Reset(Context->GetCurrentlyPressedKeys().GetSize());

    return;
}

bool Jafg::LPlayerInput::IsNewDown(const LKey Key) const
{
    const LSurface* Context = this->GetCheckedPrimaryContext();
    return Context->GetCurrentlyPressedKeys().Contains(Key) && (Context->GetLastFramePressedKeys().Contains(Key) == false);
}

void Jafg::LPlayerInput::DispatchInputDelegates()
{
    LLocalPlayer* LocalPlayer = this->GetCheckedLocalPlayer();
    if (!LocalPlayer->DoesPossess() || !LocalPlayer->HasPrimarySurface())
    {
        return;
    }
    check( LocalPlayer->GetPossessed()->GetWorld() )
    LWorld* TargetWorld = LocalPlayer->GetPossessed()->GetWorld();
    LSurface* PrimarySurface = LocalPlayer->GetPrimarySurface();

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::W))
    {
        TargetWorld->MainCamera->ProcessKeyboard(FORWARD, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::S))
    {
        TargetWorld->MainCamera->ProcessKeyboard(BACKWARD, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::A))
    {
        TargetWorld->MainCamera->ProcessKeyboard(LEFT, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::D))
    {
        TargetWorld->MainCamera->ProcessKeyboard(RIGHT, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Q))
    {
        TargetWorld->MainCamera->ProcessKeyboard(DOWN, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::E))
    {
        TargetWorld->MainCamera->ProcessKeyboard(UP, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Escape))
    {
        if (this->IsNewDown(EKeys::Escape))
        {
            TargetWorld->bShowMouse = !TargetWorld->bShowMouse;
            TargetWorld->FirstTimeMouseScroll = true;
            LocalPlayer->GetPrimarySurface()->SetInputMode(TargetWorld->bShowMouse);
        }
    }

    {
        const LRawInput* const MouseX = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseX);
        const LRawInput* const MouseY = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseY);

        if (PrimarySurface->GetCurrentlyPressedKeys().IsEmpty() == false)
        {
            __nop();
        }

        if (MouseX != nullptr && MouseY != nullptr)
        {
            TargetWorld->MouseCallback(MouseX->Value, MouseY->Value);
        }
        else if (MouseX != nullptr)
        {
            TargetWorld->MouseCallback(MouseX->Value, 0.0f);
        }
        else if (MouseY != nullptr)
        {
            TargetWorld->MouseCallback(0.0f, MouseY->Value);
        }
    }

    {
        const LRawInput* const MouseWheelAxis = PrimarySurface->GetCurrentlyPressedKeys().FindRef(EKeys::MouseWheelAxis);
        if (MouseWheelAxis != nullptr)
        {
            TargetWorld->ScrollCallback(MouseWheelAxis->Value);
        }
    }

    return;
}

Jafg::LLocalPlayer* Jafg::LPlayerInput::GetLocalPlayer() const
{
    checkSlow( GEngine )
    return GEngine->GetLocalPlayer();
}

Jafg::LLocalPlayer* Jafg::LPlayerInput::GetCheckedLocalPlayer() const
{
    checkSlow( GEngine )
    return GEngine->GetCheckedLocalPlayer();
}

Jafg::LLocalPlayer* Jafg::LPlayerInput::GetPanickedLocalPlayer() const
{
    checkSlow( GEngine )
    return GEngine->GetPanickedLocalPlayer();
}

Jafg::LSurface* Jafg::LPlayerInput::GetPrimaryContext() const
{
    return this->GetCheckedLocalPlayer()->GetPrimarySurface();
}

Jafg::LSurface* Jafg::LPlayerInput::GetCheckedPrimaryContext() const
{
    if (LSurface* Surface = this->GetCheckedLocalPlayer()->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    check( false && "Could not find primary context." )

    return nullptr;
}

Jafg::LSurface* Jafg::LPlayerInput::GetPanickedPrimaryContext() const
{
    if (LSurface* Surface = this->GetPanickedLocalPlayer()->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    panic( "Could not find primary context." )

    return nullptr;
}
