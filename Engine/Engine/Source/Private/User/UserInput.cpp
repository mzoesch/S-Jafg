// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "User/UserInput.h"
#include "Core/Application.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Framework/Pawn.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include <intrin.h>

void Jafg::LUserInput::BeginNewFrame()
{
    LSurface* Context = this->GetCheckedPrimaryContext();

    Context->GetCurrentlyPressedKeys().SwapBuffers(Context->GetLastFramePressedKeys());
    Context->GetCurrentlyPressedKeys().Reset(Context->GetCurrentlyPressedKeys().GetSize());

    return;
}

bool Jafg::LUserInput::IsNewDown(const LKey Key) const
{
    const LSurface* Context = this->GetCheckedPrimaryContext();
    return Context->GetCurrentlyPressedKeys().Contains(Key) && (Context->GetLastFramePressedKeys().Contains(Key) == false);
}

void Jafg::LUserInput::DispatchInputDelegates()
{
    LLocalEgo* LocalEgo = this->GetCheckedLocalEgo();
    if (!LocalEgo->DoesPossess() || !LocalEgo->HasPrimarySurface())
    {
        return;
    }
    check( LocalEgo->GetPossessed()->GetWorld() )
    LWorld* TargetWorld = LocalEgo->GetPossessed()->GetWorld();
    LSurface* PrimarySurface = LocalEgo->GetPrimarySurface();

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::W))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(FORWARD, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::S))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(BACKWARD, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::A))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(LEFT, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::D))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(RIGHT, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Q))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(DOWN, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::E))
    {
        LocalEgo->GetPossessed()->GetPossessed()->ProcessKeyboard(UP, Application::GetDeltaTimeAsFloat());
    }

    if (PrimarySurface->GetCurrentlyPressedKeys().Contains(EKeys::Escape))
    {
        if (this->IsNewDown(EKeys::Escape))
        {
            TargetWorld->bShowMouse = !TargetWorld->bShowMouse;
            TargetWorld->FirstTimeMouseScroll = true;
            LocalEgo->GetPrimarySurface()->SetInputMode(TargetWorld->bShowMouse);
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

Jafg::LLocalEgo* Jafg::LUserInput::GetLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetCheckedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetCheckedLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetPanickedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetPanickedLocalEgo();
}

Jafg::LSurface* Jafg::LUserInput::GetPrimaryContext() const
{
    return this->GetCheckedLocalEgo()->GetPrimarySurface();
}

Jafg::LSurface* Jafg::LUserInput::GetCheckedPrimaryContext() const
{
    if (LSurface* Surface = this->GetCheckedLocalEgo()->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    check( false && "Could not find primary context." )

    return nullptr;
}

Jafg::LSurface* Jafg::LUserInput::GetPanickedPrimaryContext() const
{
    if (LSurface* Surface = this->GetPanickedLocalEgo()->GetPrimarySurface(); Surface)
    {
        return Surface;
    }

    panic( "Could not find primary context." )

    return nullptr;
}
