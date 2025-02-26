// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Platform/Surface.h"
#include "Platform/PlatformMisc.h"
#include "Widgets/Viewport.h"

void Jafg::LSurface::Initialize()
{
    check( this->IsValid() == false )

    this->SurfaceViewport = new LViewport();
    this->SurfaceViewport->Initialize();

    LOG_VERBOSE(LogSurface, "Created surface viewport.")
    LOG_INFO(LogSurface, "Platform stats:")
    LOG_INFO(LogSurface, " - Physical monitors({}).", GPlatformMisc->NumberOfPhysicalViewports)
    for (const LPhysicalViewport& Viewport : GPlatformMisc->PhysicalViewports)
    {
        LOG_INFO(LogSurface, "  - {}", Viewport.ToString())
    }

    return;
}

void Jafg::LSurface::OnClear()
{
    if (this->SurfaceViewport)
    {
        this->SurfaceViewport->OnClear();
    }

    return;
}

void Jafg::LSurface::OnUpdate()
{
    this->SurfaceViewport->Draw();
}

void Jafg::LSurface::TearDown()
{
    if (ensure(this->SurfaceViewport))
    {
        this->SurfaceViewport->TearDown();
        delete this->SurfaceViewport;
        this->SurfaceViewport = nullptr;
    }

    return;
}

void Jafg::LSurface::BeginNewFrame()
{
    this->PlatformInput.Empty();

    this->DownKeys.SwapBuffers(this->LastFrameDownKeys);
    this->DownKeys.Reset(this->DownKeys.GetSize());

    this->PlatformRepeatedKey.Reset();

    return;
}

void Jafg::LSurface::PollVirtualInputs()
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

void Jafg::LSurface::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    this->InputMode   = InMode;
    this->bShowCursor = bInShowCursor;

    return;
}

bool Jafg::LSurface::IsKeyDown(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey);
}

bool Jafg::LSurface::IsNewKeyDown(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) && (this->GetLastFramePressedKeys().Contains(InKey) == false);
}

bool Jafg::LSurface::IsKeyUp(const LKey InKey) const
{
    return this->GetCurrentlyPressedKeys().Contains(InKey) == false && this->GetLastFramePressedKeys().Contains(InKey);
}
