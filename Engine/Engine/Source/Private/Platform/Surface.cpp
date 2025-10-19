// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Framework/PersonaController.h"
#include "Platform/PlatformMisc.h"
#include "Widgets/Viewport.h"
#include "Engine/Engine.h"

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
                        if (LRawInput* RealKey { algo::find_pointer(this->GetCurrentlyPressedKeys(), this->GetCurrenRepeatedKeyInQuestion(), &LRawInput::Key) })
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
    algo::orphan(&this->PlatformInput);

    this->DownKeys.swap(this->LastFrameDownKeys);
    this->DownKeys.clear();

    return;
}

void Jafg::LSurfaceBase::PollVirtualInputs()
{
    for (const LRawInput& Input : this->VirtualInput)
    {
        if (algo::contains(this->DownKeys, Input.Key, &LRawInput::Key) == false)
        {
            this->AddKeyDown(Input);
        }
    }

    algo::orphan(&this->VirtualInput);

    return;
}

void Jafg::LSurfaceBase::SetInputMode(const EInputMode::Type InMode, const bool bInShowCursor)
{
    this->InputMode   = InMode;
    this->bShowCursor = bInShowCursor;

    return;
}

void Jafg::LSurfaceBase::Possess(APersonaController* NewController)
{
    APersonaController* OldController{ this->Controller };
    this->Controller = NewController;

    if (OldController)
    {
        OldController->SetSurface(nullptr);
    }
    if (this->Controller)
    {
        this->Controller->SetSurface(this->AsSurface());
    }

    if (this->Controller)
    {
        if (auto const* World{ this->Controller->GetWorldChecked() }; World->IsUnderlyingLevelValid())
        {
            this->SetInputMode(World->GetUnderlyingLevelChecked().InputMode, World->GetUnderlyingLevelChecked().bShowMouseCursor);
            this->GetViewport().SetBackgroundColor(World->GetUnderlyingLevelChecked().BackgroundColor);
        }
    }

    this->GetLocalEgo().ForEachMutableSubsystem([OldController, NewController](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaControllerPossessed(OldController, NewController);
    });

    return;
}

Jafg::LEngine& Jafg::LSurfaceBase::GetEngine() const noexcept
{
    check( GEngine && "Absence of GEngine while a surface exists is undefined behavior." )
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::LSurfaceBase::GetLocalEgo() const noexcept
{
    return this->GetEngine().GetLocalEgo();
}
