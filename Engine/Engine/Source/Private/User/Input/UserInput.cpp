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

void Jafg::LUserInput::DispatchInputDelegates(LSurface& Surface)
{
    TArray TriggeredKeys{ Surface.GetTriggeredKeys() };
    TArray OngoingKeys{ Surface.GetOngoingKeys() };
    TArray CompletedKeys{ Surface.GetCompletedKeys() };

    this->DispatchInputDelegatesForKeyCategory(Surface, &TriggeredKeys, EInputActionTrigger::Triggered);
    this->DispatchInputDelegatesForKeyCategory(Surface, &OngoingKeys,   EInputActionTrigger::Ongoing);
    this->DispatchInputDelegatesForKeyCategory(Surface, &CompletedKeys, EInputActionTrigger::Completed);

    return;
}

bool Jafg::LUserInput::ActivateContext(LName Name, LSize Where /* = INDEX_NONE */) noexcept
{
    check( Name.IsSet() )

    if (algo::contains(this->ActiveContexts, Name))
    {
        LOG_VERBOSE(LogUserInput, "Context [{}] is already active. Cannot activate.", Name)
        return false;
    }

    LOG_VERBOSE(LogUserInput, "Activating context [{}].", Name)
    if (Where == static_cast<decltype(Where)>(INDEX_NONE) || Where >= this->ActiveContexts.size())
    {
        this->ActiveContexts.emplace_back(Name);
    }
    else
    {
        this->ActiveContexts.insert(this->ActiveContexts.begin() + Where, Name);
    }

    return true;
}

bool Jafg::LUserInput::ActivateContexts(TArray<LUserInputContext const*> const& Contexts, LSize Where) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &Where, &bOut](LUserInputContext const* Context)
    {
        if (this->ActivateContext(Context->GetName(), Where))
        {
            bOut = true;
            ++Where;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::ActivateContexts(TArray<LName> const& Names, LSize Where) noexcept
{
    bool bOut{ false };

    algo::for_each(Names, [this, &Where, &bOut](LName Name)
    {
        if (this->ActivateContext(Name, Where))
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
        if (this->ActivateContext(GET_NAME(Name), Where))
        {
            bOut = true;
            ++Where;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateContext(LName Name) noexcept
{
    check( Name.IsSet() )

    if (algo::contains(this->ActiveContexts, Name) == false)
    {
        LOG_VERBOSE(LogUserInput, "Context [{}] is not active. Cannot deactivate.", Name)
        return false;
    }

    LOG_VERBOSE(LogUserInput, "Deactivating context [{}].", Name)
    auto Removed{ algo::erase(&this->ActiveContexts, Name) };
    check( Removed == 1 )

    return true;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LUserInputContext const*> const& Contexts) noexcept
{
    bool bOut { false };

    algo::for_each(Contexts, [this, &bOut](LUserInputContext const* InContext)
    {
        if (this->DeactivateContext(InContext->GetName()))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

bool Jafg::LUserInput::DeactivateContexts(TArray<LName> const& Contexts) noexcept
{
    bool bOut{ false };

    algo::for_each(Contexts, [this, &bOut](LName Name)
    {
        if (this->DeactivateContext(Name))
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
        if (this->DeactivateContext(GET_NAME(Name)))
        {
            bOut = true;
        }

        return;
    });

    return bOut;
}

void Jafg::LUserInput::PushContexts(const bool bEmpty /* = true */) noexcept
{
    if (bEmpty)
    {
        this->ContextStack.emplace_back(std::move(this->ActiveContexts));
        check( this->ActiveContexts.empty() )
    }
    else
    {
        TArray Ctx{ this->ActiveContexts };
        this->ContextStack.emplace_back(std::move(Ctx));
        check( this->ContextStack.back().size() == this->ActiveContexts.size() )
    }

    LOG_VERBOSE(LogUserInput, "Pushed [{}] active contexts onto the stack.", this->ContextStack.back().size())

    return;
}

bool Jafg::LUserInput::PopContexts() noexcept
{
    bool bRet;

    if (this->ContextStack.empty())
    {
        bRet = false;
    }
    else
    {
        bRet = true;

        this->ActiveContexts = std::move(this->ContextStack.back());
        check( this->ContextStack.back().empty() )
        this->ContextStack.pop_back();
    }

    LOG_VERBOSE(LogUserInput, "Popped [{}] active contexts from the stack.", this->ActiveContexts.size())

    return bRet;
}

void Jafg::LUserInput::DispatchInputDelegatesForKeyCategory(LSurface& Surface, TArray<LRawInput>* Inputs, EInputActionTrigger::Type TriggerType)
{
    STAT_QUICK_CYCLE_START(Lal::SprintF("{}{}", LAL_PRETTY_FUNCTION, LexToString(TriggerType)))

    check( Inputs )
    check( TriggerType != EInputActionTrigger::None )

    LUserInputRegistry const& Registry{ Surface.GetLocalEgo().GetUserInputRegistry() };

    for (auto ContextName : this->ActiveContexts)
    {
        auto* Context{ Registry.GetContextByNameChecked(ContextName) };

        for (auto const& Action : Context->GetMappedActions())
        {
            for (auto const& Trigger : Action.Triggers)
            {
                if (Trigger.Type != TriggerType)
                {
                    continue;
                }

                LInputActionValue Value{ Registry.GetActionByNameChecked(Action.Action)->GetCategory() };
                for (auto It{ Inputs->begin() }; It != Inputs->end();)
                {
                    LInputActionValue::Axis3D Magnitude;

                    if (algo::contains(Trigger.Keys, It->Key) == false)
                    {
                        if (algo::contains(Trigger.Keys, EKeys::MouseXY) == false)
                        {
                            ++It;
                            continue;
                        }

                        if (It->Key == EKeys::MouseX)
                        {
                            Magnitude = LInputActionValue::Axis3D{It->Value, 0.0f, 0.0f};
                        }
                        else if (It->Key == EKeys::MouseY)
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
                        Magnitude = LInputActionValue::Axis3D{It->Value, 0.0f, 0.0f};
                    }

                    for (auto& Modifier : Trigger.Modifiers)
                    {
                        Magnitude = Modifier->ApplyModifier(Magnitude);
                    }
                    Value += Magnitude;

                    Inputs->erase(It);

                    continue;
                }

                if (Value.IsNonZero())
                {
                    Action.Callback(Surface.GetViewport(), Value);
                }

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}
