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
#if PLATFORM_LINUX
    if (this->IsPlatformSupportsRepeatedKey() == false)
    {
        this->bThisFrameRepeatedKeyDown = false;
        if (this->IsCurrenRepeatedKeyInQuestionValid())
        {
            if (this->IsKeyDown(this->GetCurrenRepeatedKeyInQuestion()))
            {
                if
                (
                    Application::GetTimeDiffFromNow(this->LastPressTimePoint) >= this->RepeatedDelay
                )
                {
                    if (this->RepeatedBufferTime >= this->RepeatedRate)
                    {
                        this->RepeatedBufferTime -= this->RepeatedRate;
                        this->bThisFrameRepeatedKeyDown = true;
                        if (LRawInput* RealKey { this->GetCurrentlyPressedKeys().FindRef(this->GetCurrenRepeatedKeyInQuestion()) })
                        {
                            RealKey->bRepeated = true;
                        }
                        else
                        {
                            LOG_ERROR(LogSurface, "Key [{}] is not in the currently pressed keys.", this->GetCurrenRepeatedKeyInQuestion())
                        }
                        this->EmulateRepeatedContentForBufferedInput();
                    }

                    this->RepeatedBufferTime += Application::GetDeltaTimeAsFloat();
                }
            }
            else
            {
                this->LastNewKey = EKeys::Unresolved;
                this->RepeatedBufferTime = this->RepeatedRate;
            }
        }
    }
#endif /* PLATFORM_LINUX */

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

    this->DownKeys.SwapBuffers(&this->LastFrameDownKeys);
    this->DownKeys.Reset(this->DownKeys.GetSize());

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
