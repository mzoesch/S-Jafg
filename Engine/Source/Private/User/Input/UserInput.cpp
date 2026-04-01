// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/InputAction.h"
#include "User/Input/InputActionValue.h"
#include "Stats/Stats.h"

LString Jafg::LexToString(EInputActionCategory::Type InType)
{
    switch (InType)
    {
    case EInputActionCategory::None:    { return "None"; }
    case EInputActionCategory::Boolean: { return "Boolean"; }
    case EInputActionCategory::Axis1D:  { return "Axis1D"; }
    case EInputActionCategory::Axis2D:  { return "Axis2D"; }
    case EInputActionCategory::Axis3D:  { return "Axis3D"; }
    }

    return "Invalid";
}

void Jafg::LUserInput::DispatchInputDelegates(APersonaController& ActingController)
{
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Surface->GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Triggered);
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Surface->GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Ongoing);
    this->DispatchInputDelegatesForKeyCategory(ActingController, &this->Surface->GetMutableUnconsumedInputsDangerous(), EInputActionTriggerBits::Completed);
}

bool Jafg::LUserInput::ActivateContext(LUserInputTag Tag, LSize Where /* = INDEX_NONE */) noexcept
{
    check( Tag.IsSet() )

    if (algo::contains(this->ActiveContexts, Tag))
    {
        LOG_VERBOSE(LogUserInput, "Context [{}] is already active. Cannot activate.", Tag.ToString())
        return false;
    }

    LOG_VERBOSE(LogUserInput, "Activating context [{}].", Tag)
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

bool Jafg::LUserInput::ActivateContexts(TArray<LUserInputContext const*> const& Contexts, LSize Where) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &Where, &bOut](LUserInputContext const* Context)
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

bool Jafg::LUserInput::ActivateContexts(TArray<LUserInputTag> const& Tags, LSize Where) noexcept
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

bool Jafg::LUserInput::ActivateContexts(TArray<LStringView> const& Names, LSize Where) noexcept
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
        LOG_VERBOSE(LogUserInput, "Context [{}] is not active. Cannot deactivate.", Tag)
        return false;
    }

    LOG_VERBOSE(LogUserInput, "Deactivating context [{}].", Tag)
    auto Removed{ algo::erase(&this->ActiveContexts, Tag) };
    check( Removed == 1 )

    return true;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LUserInputContext const*> const& Contexts) noexcept
{
    bool bOut { false };

    algo::for_each(Contexts, [this, &bOut](LUserInputContext const* InContext)
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

void Jafg::LUserInput::PushContexts(const bool bEmpty /* = true */) noexcept
{
    check(this->Surface)

    if (bEmpty)
    {
        this->ContextStack.emplace_back(this->Surface->GetInputMode(), std::move(this->ActiveContexts));
        check(this->ActiveContexts.empty())
    }
    else
    {
        this->ContextStack.emplace_back(this->Surface->GetInputMode(), this->ActiveContexts);
        check(this->ContextStack.back().second.size() == this->ActiveContexts.size())
    }

    LOG_VERBOSE(LogUserInput, "Pushed [{}] active contexts onto the stack.", this->ContextStack.back().second.size())

    return;
}

TOptional<Jafg::EInputMode> Jafg::LUserInput::PopContexts() noexcept
{
    if (this->ContextStack.empty())
    {
        return {};
    }

    this->ActiveContexts = std::move(this->ContextStack.back().second);
    check(this->ContextStack.back().second.empty())

    LOG_VERBOSE(LogUserInput, "Popped [{}] active contexts from the stack.", this->ActiveContexts.size())
    EInputMode Result{this->ContextStack.back().first};
    this->ContextStack.pop_back();

    return Result;
}

void Jafg::LUserInput::DispatchInputDelegatesForKeyCategory(APersonaController& ActingController, TArray<LRawInput>* Inputs, EInputActionTriggerBits TriggerMask)
{
    STAT_QUICK_CYCLE_START(Jafg::SprintF("{}{}", JAFG_PRETTY_FUNCTION_NAME, LexToString(TriggerMask)))

    check(Inputs)
    check(TriggerMask != EInputActionTriggerBits::Identity)

    LUserInputRegistry const& Registry{this->Surface->GetLocalEgo().GetUserInputRegistry()};
    for (auto ContextName : this->ActiveContexts)
    {
        auto& Context{*Registry.GetContextByNameChecked(ContextName)};

        for (auto const& Action : Context.GetMappedActions())
        {
            for (auto const& Trigger : Action.Triggers)
            {
                if (!(Trigger.TriggerFlags & TriggerMask))
                {
                    continue;
                }

                //# TODO: Check that if a key is doubly bound and triggered: Will this not increase the Value by two?
                LInputActionValue Value{Registry.GetActionByNameChecked(Action.ActionTag)->GetCategory()};
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

                    LInputActionValue::Axis3D Magnitude;
                    if (algo::contains(Trigger.Keys, It->PhysicalKey))
                    {
                        Magnitude = LInputActionValue::Axis3D{It->Value, 0.0f, 0.0f};
                    }
                    else
                    {
                        if (algo::contains(Trigger.Keys, LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseXY)))
                        {
                            if (It->PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseX))
                            {
                                Magnitude = LInputActionValue::Axis3D{It->Value, 0.0f, 0.0f};
                            }
                            else if (It->PhysicalKey == LPhysicalKey::FromLogical(ENamedPhysicalKey::MouseY))
                            {
                                Magnitude = LInputActionValue::Axis3D{0.0f, It->Value, 0.0f};
                            }
                            else
                            {
                                ++It;
                                continue;
                            }
                        }
                        else
                        {
                            ++It;
                            continue;
                        }
                    }

                    for (auto& Modifier : Trigger.Modifiers)
                    {
                        Magnitude = Modifier->ApplyModifier(Magnitude);
                    }
                    Value += Magnitude;

                    // TODO: Add bConsumes as an option?
                    It = Inputs->erase(It);

                    continue;
                }

                if (Value.IsNonZero())
                {
                    if (auto Result{Action.Callback({
                        .Viewport = this->Surface->GetViewport(),
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

    return;
}
