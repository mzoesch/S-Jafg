// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/InputAction.h"
#include "User/Input/InputActionValue.h"

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

bool Jafg::LUserInput::IsNewDown(const LKey Key) const
{
    return this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->IsNewKeyDown(Key);
}

void Jafg::LUserInput::DispatchInputDelegates()
{
    TArray<LRawInput> TriggeredKeys = this->GetTriggeredKeys();
    TArray<LRawInput> OngoingKeys   = this->GetOngoingKeys();
    TArray<LRawInput> CompletedKeys = this->GetCompletedKeys();

    this->DispatchInputDelegatesForKeyCategory(&TriggeredKeys, EInputActionTrigger::Triggered);
    this->DispatchInputDelegatesForKeyCategory(&OngoingKeys,   EInputActionTrigger::Ongoing);
    this->DispatchInputDelegatesForKeyCategory(&CompletedKeys, EInputActionTrigger::Completed);

    return;
}

Jafg::LLocalEgo& Jafg::LUserInput::GetLocalEgo()
{
    check( GEngine && "Absence of GEngine is undefined behavior here." )
    check( &GEngine->GetLocalEgo().GetUserInput() == this )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo const& Jafg::LUserInput::GetLocalEgo() const
{
    check( GEngine && "Absence of GEngine is undefined behavior here." )
    check( &GEngine->GetLocalEgo().GetUserInput() == this )
    return GEngine->GetLocalEgo();
}

const Jafg::LInputAction* Jafg::LUserInput::RegisterAction(LInputAction&& InAction)
{
    if (algo::contains(this->RegisteredActions, InAction.GetName(), &LInputAction::GetName))
    {
        LOG_WARNING(LogUserInput, "Action [{}] was already registered.", InAction.GetName())
        return nullptr;
    }

    this->RegisteredActions.emplace_back(std::make_unique<LInputAction>(std::move(InAction)));

    return this->RegisteredActions.back().get();
}

Jafg::LUserInputContext* Jafg::LUserInput::RegisterContext(LUserInputContext&& Context, const bool bMakeActive /* = false */)
{
    if (algo::contains(this->RegisteredContexts, Context, algo::unique_deref{}))
    {
        LOG_WARNING(LogUserInput, "Context [{}] was already registered.", Context.GetName())
        return nullptr;
    }

    this->RegisteredContexts.emplace_back(std::make_unique<LUserInputContext>(std::move(Context)));

    if (bMakeActive)
    {
        this->ActiveContexts.emplace_back(this->RegisteredContexts.back().get());
    }

    return this->RegisteredContexts.back().get();
}

bool Jafg::LUserInput::ActivateContext(LUserInputContext* InContext)
{
    if (InContext)
    {
        if (algo::contains(this->ActiveContexts, InContext))
        {
            LOG_VERBOSE(LogUserInput, "Context [{}] is already active. Cannot activate.", InContext->GetName())
            return false;
        }

        LOG_VERBOSE(LogUserInput, "Activating context [{}].", InContext->GetName())
        this->ActiveContexts.emplace_back(InContext);
        return true;
    }

    return false;
}

bool Jafg::LUserInput::DeactivateContext(LUserInputContext* InContext)
{
    if (InContext)
    {
        if (algo::contains(this->ActiveContexts, InContext) == false)
        {
            LOG_VERBOSE(LogUserInput, "Context [{}] is not active. Cannot deactivate.", InContext->GetName())
            return false;
        }

        LOG_VERBOSE(LogUserInput, "Deactivating context [{}].", InContext->GetName())
        const i32 Removed = algo::erase(&this->ActiveContexts, InContext);
        check( Removed == 1 )
        return true;
    }

    return false;
}


i32 Jafg::LUserInput::DeactivateAllContexts(TArray<LUserInputContext*>* OutActiveContexts /* = nullptr */)
{
    const auto Out { this->ActiveContexts.size() };

    if (OutActiveContexts)
    {
        *OutActiveContexts = std::move(this->ActiveContexts);
    }

    LOG_VERBOSE(LogUserInput, "Deactivating all contexts [{}].", Out)
    algo::orphan(&this->ActiveContexts);
    return static_cast<i32>(Out);
}

void Jafg::LUserInput::PushContexts(const bool bEmpty /* = true */)
{
    if (bEmpty)
    {
        this->ContextStack.emplace_back(std::move(this->ActiveContexts));
        check( this->ActiveContexts.empty() )
    }
    else
    {
        TArray<LUserInputContext*> Ctx; Ctx = this->ActiveContexts;
        this->ContextStack.emplace_back(std::move(Ctx));
        check( this->ContextStack.back().size() == this->ActiveContexts.size() )
    }

    LOG_VERBOSE(LogUserInput, "Pushed [{}] active contexts onto the stack.", this->ContextStack.back().size())

    return;
}

bool Jafg::LUserInput::PopContexts()
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
        this->ContextStack.pop_back();
    }

    LOG_VERBOSE(LogUserInput, "Popped [{}] active contexts from the stack.", this->ActiveContexts.size())

    return bRet;
}

TArray<Jafg::LRawInput> Jafg::LUserInput::GetTriggeredKeys() const
{
    TArray<LRawInput> TriggeredKeys;

    for (const LRawInput& Key : this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys())
    {
        if (algo::contains(this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetLastFramePressedKeys(), Key) == false)
        {
            TriggeredKeys.emplace_back(Key);
        }

        continue;
    }

    return TriggeredKeys;
}

const TArray<Jafg::LRawInput>& Jafg::LUserInput::GetOngoingKeys() const
{
    return this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys();
}

TArray<Jafg::LRawInput> Jafg::LUserInput::GetCompletedKeys() const
{
    TArray<LRawInput> CompletedKeys;

    for (const LRawInput& Key : this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetLastFramePressedKeys())
    {
        if (algo::contains(this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys(), Key) == false)
        {
            CompletedKeys.emplace_back(Key);
        }

        continue;
    }

    return CompletedKeys;
}

bool Jafg::LUserInput::HasBufferedPlatformInput() const
{
    check( &this->GetLocalEgo().GetUserInput() == this )
    return this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->HasBufferedPlatformInput();
}

const TArray<LString>& Jafg::LUserInput::GetBufferedPlatformInput() const
{
    check( &this->GetLocalEgo().GetUserInput() == this )
    return this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetBufferedPlatformInput();
}

LString Jafg::LUserInput::GetBufferedPlatformInputAsStr() const
{
    check( &this->GetLocalEgo().GetUserInput() == this )
    return this->GetLocalEgo().GetFrontend().GetFocusedSurfaceChecked()->GetBufferedPlatformInputAsStr();
}

void Jafg::LUserInput::DispatchInputDelegatesForKeyCategory(TArray<LRawInput>* InRawInputs, const EInputActionTrigger::Type InActionTriggerType)
{
    check( InRawInputs )

    for (LUserInputContext* Context : this->ActiveContexts)
    {
        for (const LInputMappedAction& MappedAction : Context->GetMappedActions())
        {
            for (const LInputMappedAction::LTrigger& Trigger : MappedAction.Triggers)
            {
                if (Trigger.Type != InActionTriggerType)
                {
                    continue;
                }

                LInputActionValue Value = MappedAction.Action->GetCategory();

                for (TArray<LRawInput>::size_type Idx { 0 }; Idx < InRawInputs->size();)
                {
                    const LRawInput& RawInput = (*InRawInputs)[Idx];

                    LInputActionValue::Axis3D Magnitude;

                    if (algo::contains(Trigger.Keys, RawInput.Key) == false)
                    {
                        if (algo::contains(Trigger.Keys, EKeys::MouseXY) == false)
                        {
                            ++Idx;
                            continue;
                        }

                        if (RawInput.Key == EKeys::MouseX)
                        {
                            Magnitude = {RawInput.Value, 0.0f, 0.0f};
                        }
                        else if (RawInput.Key == EKeys::MouseY)
                        {
                            Magnitude = {0.0f, RawInput.Value, 0.0f};
                        }
                        else
                        {
                            ++Idx;
                            continue;
                        }
                    }
                    else
                    {
                        Magnitude = {RawInput.Value, 0.0f, 0.0f};
                    }

                    for (auto& Modifier : Trigger.Modifiers)
                    {
                        Magnitude = Modifier->ApplyModifier(Magnitude);
                    }
                    Value += Magnitude;

                    InRawInputs->erase(InRawInputs->begin() + Idx);

                    continue;
                }

                if (Value.IsNonZero())
                {
                    MappedAction.Callback(Value);
                }

                continue;
            }

            continue;
        }

        continue;
    }

    return;
}
