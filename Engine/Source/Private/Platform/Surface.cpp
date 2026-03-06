// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Framework/PersonaController.h"
#include "Platform/PlatformMisc.h"
#include "Widgets/Viewport.h"
#include "Engine/Engine.h"

void Jafg::LSurfaceBase::BeginNewFrame()
{
    algo::orphan(&this->PlatformInput);

    this->DownKeys.swap(this->LastFrameDownKeys);
    this->DownKeys.clear();

    this->AsSurface()->PollPlatformEvents();

    for (auto const& Input : this->VirtualInput)
    {
        if (algo::contains(this->DownKeys, Input.Key, &LRawInput::Key) == false)
        {
            this->AddKeyDown(Input);
        }
    }
    this->VirtualInput.clear();

    return;
}

void Jafg::LSurfaceBase::Tick()
{
#if PLATFORM_LINUX
    // if (this->IsPlatformSupportsRepeatedKey() == false)
    // {
    //     this->bThisFrameRepeatedKeyDown = false;
    //     if (this->IsCurrenRepeatedKeyInQuestionValid())
    //     {
    //         if (this->IsKeyDown(this->GetCurrenRepeatedKeyInQuestion()))
    //         {
    //             if
    //             (
    //                 Application::GetTimeDiffFromNow(this->LastPressTimePoint) >= this->RepeatedDelay
    //             )
    //             {
    //                 if (this->RepeatedBufferTime >= this->RepeatedRate)
    //                 {
    //                     this->RepeatedBufferTime -= this->RepeatedRate;
    //                     this->bThisFrameRepeatedKeyDown = true;
    //                     if (LRawInput* RealKey { algo::find_pointer(this->GetCurrentlyPressedKeys(), this->GetCurrenRepeatedKeyInQuestion(), &LRawInput::Key) })
    //                     {
    //                         RealKey->bRepeated = true;
    //                     }
    //                     else
    //                     {
    //                         LOG_ERROR(LogSurface, "Key [{}] is not in the currently pressed keys.", this->GetCurrenRepeatedKeyInQuestion())
    //                     }
    //                     this->EmulateRepeatedContentForBufferedInput();
    //                 }
    //
    //                 this->RepeatedBufferTime += Application::GetDeltaTimeAsFloat();
    //             }
    //         }
    //         else
    //         {
    //             this->LastNewKey = EKeys::Unresolved;
    //             this->RepeatedBufferTime = this->RepeatedRate;
    //         }
    //     }
    // }
#endif /* PLATFORM_LINUX */

    const bool bCheckInput = static_cast<bool>(this->InputMode & EInputModeBits::UserInterface);

    this->GetViewport().ClearInvalidWidgets();

    if (bCheckInput)
    {
        if (this->HasMouseLocation())
        {
            this->GetViewport().DispatchInputs(*this->AsSurface(), this->GetMouseLocationValue());
        }
        else
        {
            this->GetViewport().DispatchInputs(*this->AsSurface(), LVec2D{-1.0f});
        }
    }

    if (bCheckInput == false || this->HasMouseLocation() == false)
    {
        this->GetViewport().OnMouseLeftViewport(*this->AsSurface(), bCheckInput == false);
    }

    this->SurfaceViewport.Tick();

    return;
}

void Jafg::LSurfaceBase::PossessController(APersonaController* New, const bool bKillOld /* = true */)
{
    if (this->Controller)
    {
        this->Controller->_SetOwningSurface(nullptr);
        if (bKillOld)
        {
            this->Controller->MarkAsGarbage_v2();
        }
    }

    this->Controller = New;
    if (this->Controller)
    {
        this->Controller->_SetOwningSurface(this->AsSurface());
    }

    if (this->Controller)
    {
        if (auto const& World{this->Controller->GetWorld()}; World.IsUnderlyingLevelValid())
        {
            this->AsSurface()->SetInputMode(World.GetUnderlyingLevelChecked().InputMode);
        }
    }

    this->GetLocalEgo().ForEachMutableSubsystem([New](JLocalEgoSubsystem* Subsystem)
    {
        Subsystem->OnNewPersonaController(New);
    });

    return;
}

Jafg::LEngine& Jafg::LSurfaceBase::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return *GEngine;
}

Jafg::LLocalEgo& Jafg::LSurfaceBase::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return GEngine->GetLocalEgo();
}

Jafg::LFrontend& Jafg::LSurfaceBase::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return GEngine->GetLocalEgo().GetFrontend();
}
