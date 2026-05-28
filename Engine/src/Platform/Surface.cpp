// Copyright mzoesch. All rights reserved.

#include "Platform/Surface.h"
#include "Nodes/Viewport.h"
#include "Engine/Engine.h"

void Jafg::LSurfaceBase::BeginNewFrame()
{
    this->PlatformInput.clear();
    this->DecayInputs();

    this->AsSurface()->PollPlatformEvents();

    for (auto const& Input : this->VirtualInput)
    {
        if (!algo::contains(this->RawInputs, Input.PhysicalKey, &LRawInput::PhysicalKey))
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

void Jafg::LSurfaceBase::DecayInputs()
{
    for (auto It{this->RawInputs.begin()}; It != this->RawInputs.end();)
    {
        auto& Input{*It};

        check(Input.PhysicalKey != LPhysicalKey{})
        check(Input.State != ERawInputStateBits::Identity)

        if (Input.PhysicalKey.IsAnyLogicalOf(
              ELogicalKey::MouseX, ELogicalKey::MouseY
            , ELogicalKey::MouseWheelUp, ELogicalKey::MouseWheelDown
            ))
        {
            It = this->RawInputs.erase(It);
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
            It = this->RawInputs.erase(It);
            continue;
        }

        ++It;
    }

    return;
}
