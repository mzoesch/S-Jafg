// Copyright mzoesch. All rights reserved.

#include "Player/PlayerInput.h"
#include <intrin.h>
#include "Platform/DesktopPlatform.h"
#include "Core/Application.h"
#include "Engine/World.h"
#include "Engine/Framework/Camera.h"
#include "Platform/Surface.h"

void JPlayerInput::BeginNewFrame()
{
    this->LastFrameDownKeys = this->DownKeys;
    this->DownKeys.clear();

    return;
}

bool JPlayerInput::IsNewDown(const LKey& Key) const
{
    return this->DownKeys.contains(LRawInput(Key)) && (this->LastFrameDownKeys.contains(LRawInput(Key)) == false);
}

void JPlayerInput::ProcessInput()
{
    /* Player in transit or not yet initialized in a world. */
    if (this->GetWorld() == nullptr)
    {
        return;
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::W)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(FORWARD, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::S)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(BACKWARD, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::A)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(LEFT, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::D)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(RIGHT, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::Q)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(DOWN, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::E)))
    {
        this->GetWorld()->MainCamera->ProcessKeyboard(UP, Application::GetDeltaTimeAsFloat());
    }

    if (this->GetCurrentlyPressedKeys().contains(LRawInput(EKeys::Escape)))
    {
        if (this->IsNewDown(EKeys::Escape))
        {
            this->GetWorld()->bShowMouse = !this->GetWorld()->bShowMouse;
            this->GetWorld()->FirstTimeMouseScroll = true;
            glfwSetInputMode(this->GetEngine()->GetSurface()->As<DesktopPlatform>()->GetMasterWindow(), GLFW_CURSOR,
                this->GetWorld()->bShowMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        }
    }

    {
        const std::set<LRawInput>::const_iterator MouseX = this->GetCurrentlyPressedKeys().find(LRawInput(EKeys::MouseX));
        const std::set<LRawInput>::const_iterator MouseY = this->GetCurrentlyPressedKeys().find(LRawInput(EKeys::MouseY));

        if (this->DownKeys.empty() == false)
        {
            __nop();
        }

        if (MouseX != this->GetCurrentlyPressedKeys().end() && MouseY != this->GetCurrentlyPressedKeys().end())
        {
            this->GetWorld()->MouseCallback(MouseX->Value, MouseY->Value);
        }
        else if (MouseX != this->GetCurrentlyPressedKeys().end())
        {
            this->GetWorld()->MouseCallback(MouseX->Value, 0.0f);
        }
        else if (MouseY != this->GetCurrentlyPressedKeys().end())
        {
            this->GetWorld()->MouseCallback(0.0f, MouseY->Value);
        }
    }

    {
        const std::set<LRawInput>::const_iterator MouseWheelAxis = this->GetCurrentlyPressedKeys().find(LRawInput(EKeys::MouseWheelAxis));
        if (MouseWheelAxis != this->GetCurrentlyPressedKeys().end())
        {
            this->GetWorld()->ScrollCallback(MouseWheelAxis->Value);
        }
    }

    return;
}
