// Copyright mzoesch. All rights reserved.

#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/InputAction.h"
#include "User/Input/InputActionValue.h"

Jafg::LString Jafg::LexToString(EInputActionCategory::Type InType)
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
    return this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->IsNewKeyDown(Key);
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

Jafg::LLocalEgo* Jafg::LUserInput::GetLocalEgo() const
{
    checkSlow( GEngine && GEngine->GetLocalEgo() && GEngine->GetLocalEgo()->GetUserInput() == this )
    return GEngine->GetLocalEgo();
}

const Jafg::LInputAction* Jafg::LUserInput::RegisterAction(LInputAction&& InAction)
{
    if (this->RegisteredActions.Contains(InAction.GetName()))
    {
        LOG_WARNING(LogUserInput, "Action [{}] was already registered.", InAction.GetName())
        return nullptr;
    }

    this->RegisteredActions.Emplace(Smart::EmplaceUnique<LInputAction>(std::move(InAction)));

    return *this->RegisteredActions.GetLast();
}

Jafg::LUserInputContext* Jafg::LUserInput::RegisterContext(LUserInputContext&& Context, const bool bMakeActive /* = false */)
{
    if (this->RegisteredContexts.Contains(Context))
    {
        LOG_WARNING(LogUserInput, "Context [{}] was already registered.", Context.GetName())
        return nullptr;
    }

    this->RegisteredContexts.Emplace(Smart::EmplaceUnique<LUserInputContext>(std::move(Context)));

    if (bMakeActive)
    {
        this->ActiveContexts.Emplace(*this->RegisteredContexts.GetLast());
    }

    return *this->RegisteredContexts.GetLast();
}

bool Jafg::LUserInput::ActivateContext(LUserInputContext* InContext)
{
    if (InContext)
    {
        if (this->ActiveContexts.Contains(InContext))
        {
            LOG_VERBOSE(LogUserInput, "Context [{}] is already active. Cannot activate.", InContext->GetName())
            return false;
        }

        LOG_VERBOSE(LogUserInput, "Activating context [{}].", InContext->GetName())
        this->ActiveContexts.Emplace(InContext);
        return true;
    }

    return false;
}

bool Jafg::LUserInput::DeactivateContext(LUserInputContext* InContext)
{
    if (InContext)
    {
        if (this->ActiveContexts.Contains(InContext) == false)
        {
            LOG_VERBOSE(LogUserInput, "Context [{}] is not active. Cannot deactivate.", InContext->GetName())
            return false;
        }

        LOG_VERBOSE(LogUserInput, "Deactivating context [{}].", InContext->GetName())
        const i32 Removed = this->ActiveContexts.Remove(InContext);
        check( Removed == 1 )
        return true;
    }

    return false;
}


i32 Jafg::LUserInput::DeactivateAllContexts(TArray<LUserInputContext*>* OutActiveContexts /* = nullptr */)
{
    const i32 Out = this->ActiveContexts.GetSize();

    if (OutActiveContexts)
    {
        *OutActiveContexts = std::move(this->ActiveContexts);
    }

    LOG_VERBOSE(LogUserInput, "Deactivating all contexts [{}].", Out)
    this->ActiveContexts.Empty();
    return Out;
}

void Jafg::LUserInput::PushContexts(const bool bEmpty /* = true */)
{
    if (bEmpty)
    {
        this->ContextStack.Emplace(std::move(this->ActiveContexts));
        check( this->ActiveContexts.IsEmpty() )
    }
    else
    {
        TArray<LUserInputContext*> Ctx; Ctx = this->ActiveContexts;
        this->ContextStack.Add(std::move(Ctx));
        check( this->ContextStack.GetLast()->GetSize() == this->ActiveContexts.GetSize() )
    }

    LOG_VERBOSE(LogUserInput, "Pushed [{}] active contexts onto the stack.", this->ContextStack.GetLast()->GetSize())

    return;
}

bool Jafg::LUserInput::PopContexts()
{
    bool bRet;

    if (this->ContextStack.IsEmpty())
    {
        bRet = false;
    }
    else
    {
        bRet = true;

        this->ActiveContexts = std::move(*this->ContextStack.GetLast());
        this->ContextStack.Pop();
    }

    LOG_VERBOSE(LogUserInput, "Popped [{}] active contexts from the stack.", this->ActiveContexts.GetSize())

    return bRet;
}

TArray<Jafg::LRawInput> Jafg::LUserInput::GetTriggeredKeys() const
{
    TArray<LRawInput> TriggeredKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys())
    {
        if (this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetLastFramePressedKeys().Contains(Key) == false)
        {
            TriggeredKeys.Emplace(Key);
        }

        continue;
    }

    return TriggeredKeys;
}

const TArray<Jafg::LRawInput>& Jafg::LUserInput::GetOngoingKeys() const
{
    return this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys();
}

TArray<Jafg::LRawInput> Jafg::LUserInput::GetCompletedKeys() const
{
    TArray<LRawInput> CompletedKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetLastFramePressedKeys())
    {
        if (this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetCurrentlyPressedKeys().Contains(Key) == false)
        {
            CompletedKeys.Emplace(Key);
        }

        continue;
    }

    return CompletedKeys;
}

bool Jafg::LUserInput::HasBufferedPlatformInput() const
{
    check( this->GetLocalEgo()->GetUserInput() == this )
    return this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->HasBufferedPlatformInput();
}

const TArray<Jafg::LString>& Jafg::LUserInput::GetBufferedPlatformInput() const
{
    check( this->GetLocalEgo()->GetUserInput() == this )
    return this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetBufferedPlatformInput();
}

Jafg::LString Jafg::LUserInput::GetBufferedPlatformInputAsStr() const
{
    check( this->GetLocalEgo()->GetUserInput() == this )
    return this->GetLocalEgo()->GetFrontend()->GetFocusedSurfaceChecked()->GetBufferedPlatformInputAsStr();
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

                for (TArray<LRawInput>::SizeType Idx { 0 }; Idx < InRawInputs->GetSize();)
                {
                    const LRawInput& RawInput = (*InRawInputs)[Idx];

                    LInputActionValue::Axis3D Magnitude;

                    if (Trigger.Keys.Contains(RawInput.Key) == false)
                    {
                        if (Trigger.Keys.Contains(EKeys::MouseXY) == false)
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

                    for (const LInputActionMappedTriggerModifier* Modifier : Trigger.Modifiers)
                    {
                        Magnitude = Modifier->ApplyModifier(Magnitude);
                    }
                    Value += Magnitude;

                    InRawInputs->RemoveAt(Idx);

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
