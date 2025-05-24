// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/Surface.h"
#include "Platform/PlatformMisc.h"
#include "Widgets/Viewport.h"

void Jafg::LSurfaceBase::Initialize()
{
    check( this->IsValid() == false )

    this->SurfaceViewport.Initialize();

    LOG_VERBOSE(LogSurface, "Created surface viewport.")
    LOG_INFO(LogSurface, "Platform stats:")
    LOG_INFO(LogSurface, " - Physical monitors({}).", GPlatformMisc->NumberOfPhysicalViewports)
    for (const LPhysicalViewport& Viewport : GPlatformMisc->PhysicalViewports)
    {
        LOG_INFO(LogSurface, "  - {}", Viewport.ToString())
    }

    return;
}

void Jafg::LSurfaceBase::Tick()
{
    const bool bCheckInput = this->InputMode & EInputMode::UserInterface;

    this->GetViewport().ClearInvalidWidgets();

    if (bCheckInput)
    {
        if (this->IsMouseLocationMeaningful())
        {
            this->GetViewport().DispatchInputs(*this->AsSurface(), this->GetMouseLocation());
        }
        else
        {
            this->GetViewport().DispatchInputs(*this->AsSurface(), LVector2(-1.0f));
        }
    }

    if (bCheckInput == false || this->IsMouseLocationMeaningful() == false)
    {
        this->GetViewport().OnMouseLeftViewport(*this->AsSurface(), bCheckInput == false);
    }

    this->SurfaceViewport.Tick();

    return;
}

void Jafg::LSurfaceBase::TearDown()
{
    this->SurfaceViewport.TearDown();
}

void Jafg::LSurfaceBase::BeginNewFrame()
{
    this->PlatformInput.Empty();

    this->DownKeys.SwapBuffers(this->LastFrameDownKeys);
    this->DownKeys.Reset(this->DownKeys.GetSize());

    this->PlatformRepeatedKey.Reset();

    return;
}

void Jafg::LSurfaceBase::PollVirtualInputs()
{
    for (const LRawInput& Input : this->VirtualInput)
    {
        if (this->DownKeys.Contains(Input.Key) == false)
        {
            this->AddKeyDown(Input);
        }
    }

    this->VirtualInput.Empty();

    return;
}

void Jafg::LSurfaceBase::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    this->InputMode   = InMode;
    this->bShowCursor = bInShowCursor;

    return;
}

bool Jafg::LSurfaceBase::IsKeyDown(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey);
}

bool Jafg::LSurfaceBase::IsNewKeyDown(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) && (this->GetLastFramePressedKeys().Contains(InKey) == false);
}

bool Jafg::LSurfaceBase::IsKeyUp(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) == false && this->GetLastFramePressedKeys().Contains(InKey);
}
