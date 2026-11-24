// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Framework/Surface.h"
#include "Framework/Cursor.h"
#include "User/LocalEgo.h"
#include "User/Input/InputActionValue.h"

Jafg::EModFlags Jafg::GetHeldChordModifiers(LSurfaceBase const& Surface) noexcept
{
    auto IsHeld{[&Surface](ELogicalKey Key) -> bool
    {
        auto PhysicalKey{Surface.GetFrontend().GetPhysicalKey(Key)};
        return PhysicalKey.has_value()
            && *PhysicalKey != LPhysicalKey{}
            && Surface.HasPlatformKeyState(*PhysicalKey, ERawInputStateBits::Press|ERawInputStateBits::Hold|ERawInputStateBits::Repeat);
    }};

    EModFlags Mods;
    if (IsHeld(ELogicalKey::LeftShift) || IsHeld(ELogicalKey::RightShift))         { Mods |= EModBits::Shift; }
    if (IsHeld(ELogicalKey::LeftControl) || IsHeld(ELogicalKey::RightControl))     { Mods |= EModBits::Control; }
    if (IsHeld(ELogicalKey::LeftAlt) || IsHeld(ELogicalKey::RightAlt))             { Mods |= EModBits::Alt; }
    if (IsHeld(ELogicalKey::LeftCommand) || IsHeld(ELogicalKey::RightCommand))     { Mods |= EModBits::Super; }
    return Mods;
}

void Jafg::LUserInput::_DispatchInputDelegates(APersonaController& ActingController)
{
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Viewport.GetSurface().GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Triggered);
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Viewport.GetSurface().GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Ongoing);
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Viewport.GetSurface().GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Completed);
}

bool Jafg::LUserInput::ActivateContext(LUserInputTag Tag, std::size_t Where /* = INDEX_NONE */) noexcept
{
    check( Tag.IsSet() )

    if (algo::contains(this->ActiveContexts, Tag))
    {
        LOG_TRACE(LogUserInput, "Context [{}] is already active. Cannot activate.", Tag.ToString())
        return false;
    }

    LOG_TRACE(LogUserInput, "Activating context [{}].", Tag)
    if (Where == static_cast<decltype(Where)>(INDEX_NONE) || Where >= this->ActiveContexts.size())
    {
        this->ActiveContexts.emplace_back(Tag);
    }
    else
    {
        this->ActiveContexts.insert(this->ActiveContexts.begin() + Where, Tag);
    }

    return true;
}

bool Jafg::LUserInput::ActivateContexts(TArray<LInputContext const*> const& Contexts, std::size_t Where /* = INDEX_NONE */) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &Where, &bOut](LInputContext const* Context)
    {
        if (this->ActivateContext(Context->GetTag(), Where))
        {
            bOut = true;
            ++Where;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::ActivateContexts(TArray<LUserInputTag> const& Tags, std::size_t Where /* = INDEX_NONE */) noexcept
{
    bool bOut{ false };

    algo::for_each(Tags, [this, &Where, &bOut](LUserInputTag Tag)
    {
        if (this->ActivateContext(Tag, Where))
        {
            bOut = true;
            ++Where;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::ActivateContexts(TArray<LStringView> const& Names, std::size_t Where /* = INDEX_NONE */) noexcept
{
    bool bOut{ false };

    algo::for_each(Names, [this, &Where, &bOut](LStringView Name)
    {
        if (this->ActivateContext(LUserInputTag::ToTag(Name), Where))
        {
            bOut = true;
            ++Where;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateContext(LUserInputTag Tag) noexcept
{
    check( Tag.IsSet() )

    if (algo::contains(this->ActiveContexts, Tag) == false)
    {
        LOG_TRACE(LogUserInput, "Context [{}] is not active. Cannot deactivate.", Tag)
        return false;
    }

    LOG_TRACE(LogUserInput, "Deactivating context [{}].", Tag)
    auto Removed{ algo::erase(&this->ActiveContexts, Tag) };
    check( Removed == 1 )

    return true;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LInputContext const*> const& Contexts) noexcept
{
    bool bOut { false };

    algo::for_each(Contexts, [this, &bOut](LInputContext const* InContext)
    {
        if (this->DeactivateContext(InContext->GetTag()))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LUserInputTag> const& Contexts) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &bOut](LUserInputTag Tag)
    {
        if (this->DeactivateContext(Tag))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LStringView> const& Contexts) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &bOut](LStringView Name)
    {
        if (this->DeactivateContext(LUserInputTag::ToTag(Name)))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateAllContexts() noexcept
{
    if (this->ActiveContexts.empty())
    {
        return false;
    }

    LOG_TRACE(LogUserInput, "Deactivating all [{}] active contexts.", this->ActiveContexts.size())
    this->ActiveContexts.clear();

    return true;
}

void Jafg::LUserInput::PushContexts(bool bEmpty /* = true */) noexcept
{
    if (bEmpty)
    {
        this->ContextStack.emplace_back(this->IsConsumingMouse(), std::move(this->ActiveContexts));
        check(this->ActiveContexts.empty())
    }
    else
    {
        this->ContextStack.emplace_back(this->IsConsumingMouse(), this->ActiveContexts);
        check(this->ContextStack.back().second.size() == this->ActiveContexts.size())
    }

    LOG_TRACE(LogUserInput, "Pushed [{}] active contexts onto the stack.", this->ContextStack.back().second.size())

    return;
}

std::optional<bool> Jafg::LUserInput::PopContexts() noexcept
{
    if (this->ContextStack.empty())
    {
        return {};
    }

    this->ActiveContexts = std::move(this->ContextStack.back().second);
    check(this->ContextStack.back().second.empty())

    LOG_TRACE(LogUserInput, "Popped [{}] active contexts from the stack.", this->ActiveContexts.size())
    bool Result{this->ContextStack.back().first};
    this->ContextStack.pop_back();

    return Result;
}

void Jafg::LUserInput::SetConsumeMouse(bool bConsume) noexcept
{
    check(Tasks::IsOnMasterThread())

    if (this->bConsumeMouse == bConsume)
    {
        return;
    }
    this->bConsumeMouse = bConsume;
    LOG_TRACE(LogUserInput, "Setting consume mouse to [{}].", bConsume)

    if (this->_bCurrentlyConsuming)
    {
        this->Viewport.GetSurface().SetInputMode(this->GetPreferredInputMode());
    }

    return;
}

void Jafg::LUserInput::SetConfineMouse(bool bConfine) noexcept
{
    check(Tasks::IsOnMasterThread())

    if (this->bConfineMouse == bConfine)
    {
        return;
    }
    this->bConfineMouse = bConfine;
    LOG_TRACE(LogUserInput, "Setting confine mouse to [{}].", bConfine)

    if (this->_bCurrentlyConsuming)
    {
        this->Viewport.GetSurface().SetInputMode(this->GetPreferredInputMode());
    }

    return;
}

void Jafg::LUserInput::DispatchInputDelegatesForKeyCategory(APersonaController& ActingController, TArray<LRawInput>* Inputs, EInputActionTriggerBits TriggerMask)
{
    STAT_CUSTOM_ZONE_FMT("DispatchInputDelegatesForKeyCategory-{}", serde::to_string(TriggerMask))

    check(Inputs)
    check(TriggerMask != EInputActionTriggerBits::Identity)

    LUserInputRegistry const& Registry{this->Viewport.GetSurface().GetLocalEgo().GetUserInputRegistry()};
    const EModFlags HeldMods{GetHeldChordModifiers(this->Viewport.GetSurface())};

    //# @return The magnitude, if the chord matches the input.
    auto Match{[HeldMods](LKeyChord const& Chord, LRawInput const& Input) -> std::optional<LInputActionValue::Axis3D>
    {
        if (!Chord.AreModifiersHeld(HeldMods))
        {
            return {};
        }
        if (Chord.Key == Input.PhysicalKey)
        {
            return LInputActionValue::Axis3D{Input.Value, 0.0f, 0.0f};
        }
        if (Chord.Key == LPhysicalKey::FromLogical(ELogicalKey::MouseXY))
        {
            if (Input.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseX))
            {
                return LInputActionValue::Axis3D{Input.Value, 0.0f, 0.0f};
            }
            if (Input.PhysicalKey == LPhysicalKey::FromLogical(ELogicalKey::MouseY))
            {
                return LInputActionValue::Axis3D{0.0f, Input.Value, 0.0f};
            }
        }
        return {};
    }};

    //#
    //# Chords with modifiers are more specific. They get the first chance to consume an input, so that e.g. Ctrl+S
    //# does not also trigger a binding of a plain S.
    //#
    for (const bool bModifiedPass: {true, false})
    {
        for (auto ContextName : this->ActiveContexts)
        {
            auto& Context{Registry.GetContext(ContextName)};

            for (auto const& Action : Context.GetRegisteredActions())
            {
                for (auto const& Trigger : Action.Triggers)
                {
                    if (!(Trigger.TriggerFlags & TriggerMask))
                    {
                        continue;
                    }
                    if (!algo::any_of(Trigger.Keys, [bModifiedPass](LKeyChord const& Chord){ return Chord.HasModifiers() == bModifiedPass; }))
                    {
                        continue;
                    }

                    //# TODO: Check that if a key is doubly bound and triggered: Will this not increase the Value by two?
                    LInputActionValue Value{Action.Category};
                    for (auto It{Inputs->begin()}; It != Inputs->end();)
                    {
                        if (TriggerMask == EInputActionTriggerBits::Triggered)
                        {
                            if ((It->State & ERawInputStateBits::Press) == ERawInputStateBits::Identity)
                            {
                                ++It;
                                continue;
                            }
                        }
                        else if (TriggerMask == EInputActionTriggerBits::Ongoing)
                        {
                            if ((It->State & ERawInputStateBits::Hold) == ERawInputStateBits::Identity)
                            {
                                ++It;
                                continue;
                            }
                        }
                        else if (TriggerMask == EInputActionTriggerBits::Completed)
                        {
                            if ((It->State & ERawInputStateBits::Release) == ERawInputStateBits::Identity)
                            {
                                ++It;
                                continue;
                            }
                        }

                        std::optional<LInputActionValue::Axis3D> Magnitude;
                        for (auto const& Chord : Trigger.Keys)
                        {
                            if (Chord.HasModifiers() != bModifiedPass)
                            {
                                continue;
                            }
                            if ((Magnitude = Match(Chord, *It)))
                            {
                                break;
                            }
                        }
                        if (!Magnitude)
                        {
                            ++It;
                            continue;
                        }

                        for (auto& Modifier : Trigger.Modifiers)
                        {
                            *Magnitude = Modifier->ApplyModifier(*Magnitude);
                        }
                        Value += *Magnitude;

                        // TODO: Add bConsumes as an option?
                        It = Inputs->erase(It);

                        continue;
                    }

                    if (Value.IsNonZero())
                    {
                        if (auto Result{Action.Callback({
                            .Viewport = this->Viewport,
                            .Controller = ActingController,
                            .UserInput = *this,
                            }, Value)}; Result.bDirty)
                        {
                            /* Do not proces any further. This is not optimal, as this can result in discarded inputs. */
                            return;
                        }
                    }

                    continue;
                }

                continue;
            }

            continue;
        }
    }

    return;
}
