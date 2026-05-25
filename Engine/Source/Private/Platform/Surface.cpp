// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Engine/Engine.h"

void Jafg::LSurfaceBase::BeginNewFrame()
{
    this->PlatformInput.clear();

    for (auto Idx{0uz}; Idx < this->RawInputs.size();)
    {
        auto& Input{this->RawInputs[Idx]};

        check(Input.PhysicalKey != LPhysicalKey{})
        check(Input.State != ERawInputStateBits::Identity)

        if (   Input.PhysicalKey.IsLogical(ELogicalKey::MouseXY)
            || Input.PhysicalKey.IsLogical(ELogicalKey::MouseWheelUp)
            || Input.PhysicalKey.IsLogical(ELogicalKey::MouseWheelDown)
            )
        {
            this->RawInputs.erase(this->RawInputs.begin() + Idx);
            continue;
        }

        if (Input.State & ERawInputStateBits::Press)
        {
#if JAFG_DO_CHECKS
            auto OldState{Input.State};
#endif /* JAFG_DO_CHECKS */
            Input.State &= ~ERawInputStateFlags{ERawInputStateBits::Press};
            check((Input.State & ERawInputStateBits::Press) == ERawInputStateBits::Identity)
            check((Input.State | ERawInputStateBits::Press) == OldState)
            Input.State |= ERawInputStateBits::Hold;
        }

        if (Input.State & ERawInputStateBits::Repeat)
        {
#if JAFG_DO_CHECKS
            auto OldState{Input.State};
#endif /* JAFG_DO_CHECKS */
            Input.State &= ~ERawInputStateFlags{ERawInputStateBits::Repeat};
            check((Input.State & ERawInputStateBits::Repeat) == ERawInputStateBits::Identity)
            check((Input.State | ERawInputStateBits::Repeat) == OldState)
        }

        if (Input.State & ERawInputStateBits::Release)
        {
            this->RawInputs.erase(this->RawInputs.begin() + Idx);
            continue;
        }

        ++Idx;
    }

    this->AsSurface()->PollPlatformEvents();

    for (auto const& Input : this->VirtualInput)
    {
        if (algo::contains(this->RawInputs, Input.PhysicalKey, &LRawInput::PhysicalKey) == false)
        {
            this->UpdateKeyState(Input);
        }
    }
    this->VirtualInput.clear();

    this->UnconsumedInputs = this->RawInputs;

    return;
}

void Jafg::LSurfaceBase::Tick()
{
    this->GetViewport().DispatchInputs();
    this->SurfaceViewport.Tick();

    return;
}

void Jafg::LSurfaceBase::UpdateKeyState(LRawInput const& InRawInput)
{
    check(Tasks::IsOnMasterThread())

    check(InRawInput.PhysicalKey != LPhysicalKey{})
    // This is not allowed.
    // If an action requires XY then it will be built on the spot from X and Y respectively.
    check(InRawInput.PhysicalKey != LPhysicalKey::FromLogical(ELogicalKey::MouseXY))
    // Same as above.
    // check(InRawInput.PhysicalKey != LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseWheelAxis))
    checkCode
    (
        if (   InRawInput.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelUp)
            || InRawInput.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseWheelDown)
            || InRawInput.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseX)
            || InRawInput.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseY)
            )
        {
            check(InRawInput.State == ERawInputStateBits::Press)
        }
    )

    if (auto It{algo::find(this->RawInputs, InRawInput.PhysicalKey, &LRawInput::PhysicalKey)}; It != this->RawInputs.end())
    {
        It->Mods = InRawInput.Mods;
        It->Value = InRawInput.Value;
        It->State |= InRawInput.State;
    }
    else
    {
        this->RawInputs.emplace_back(InRawInput);
    }

    return;
}

Jafg::LEngine const& Jafg::LSurfaceBase::GetEngine() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return *GEngine;
}

Jafg::LEngine& Jafg::LSurfaceBase::GetMutableEngine() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine while a surface exists is undefined behavior.")
    return *Detail::GMutableEngine;
}

Jafg::LLocalEgo const& Jafg::LSurfaceBase::GetLocalEgo() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo& Jafg::LSurfaceBase::GetMutableLocalEgo() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine while a surface exists is undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo();
}

Jafg::LFrontend const& Jafg::LSurfaceBase::GetFrontend() const noexcept
{
    check(GEngine && "Absence of GEngine while a surface exists is undefined behavior.")
    return GEngine->GetLocalEgo().GetFrontend();
}

Jafg::LFrontend& Jafg::LSurfaceBase::GetMutableFrontend() noexcept
{
    check(Detail::GMutableEngine && "Absence of GMutableEngine while a surface exists is undefined behavior.")
    return Detail::GMutableEngine->GetLocalEgo().GetFrontend();
}
