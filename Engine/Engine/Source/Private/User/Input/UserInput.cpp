// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/UserInput.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "Engine/Framework/PersonaController.h"
#include "Platform/Surface.h"
#include "User/LocalEgo.h"
#include "User/Input/InputAction.h"
#include "User/Input/InputActionValue.h"

bool Jafg::LUserInput::IsNewDown(const LKey Key) const
{
    const LSurface* Context = this->GetLocalEgo()->GetPrimarySurface();
    check( Context )
    return Context->GetCurrentlyPressedKeys().Contains(Key) && (Context->GetLastFramePressedKeys().Contains(Key) == false);
}

void Jafg::LUserInput::DispatchInputDelegates()
{
    const LLocalEgo* LocalEgo = this->GetCheckedLocalEgo();
    if (!LocalEgo->DoesPossess() || !LocalEgo->HasPrimarySurface())
    {
        return;
    }
    check( LocalEgo->GetPossessed()->GetWorld() )

    LWorld*   TargetWorld    = LocalEgo->GetPossessed()->GetWorld(); check( TargetWorld )
    LSurface* PrimarySurface = LocalEgo->GetPrimarySurface(); check( PrimarySurface )

    const TdhArray<LRawInput> TriggeredKeys = this->GetTriggeredKeys();
    const TdhArray<LRawInput> OngoingKeys   = this->GetOngoingKeys();
    const TdhArray<LRawInput> CompletedKeys = this->GetCompletedKeys();

    for (LUserInputContext* Context : this->ActiveContexts)
    {
        for (LInputMappedAction& MappedAction : Context->GetMappedActions())
        {
            if (MappedAction.Trigger == EInputActionTrigger::Triggered)
            {
                this->DispatchInputDelegatesForAction(Context, TriggeredKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EInputActionTrigger::Ongoing)
            {
                this->DispatchInputDelegatesForAction(Context, OngoingKeys, &MappedAction);
            }

            else if (MappedAction.Trigger == EInputActionTrigger::Completed)
            {
                this->DispatchInputDelegatesForAction(Context, CompletedKeys, &MappedAction);
            }

            continue;
        }

        continue;
    }

    return;
}

Jafg::LLocalEgo* Jafg::LUserInput::GetLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetCheckedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetCheckedLocalEgo();
}

Jafg::LLocalEgo* Jafg::LUserInput::GetPanickedLocalEgo() const
{
    checkSlow( GEngine )
    return GEngine->GetPanickedLocalEgo();
}

void Jafg::LUserInput::RegisterContext(LUserInputContext&& Context, const bool bMakeActive /* = false */)
{
    LUserInputContext* ContextPtr = new LUserInputContext(std::move(Context));
    this->RegisteredContexts.Add(ContextPtr);

    if (bMakeActive)
    {
        this->ActiveContexts.Add(ContextPtr);
    }

    return;
}

Jafg::TdhArray<Jafg::LRawInput> Jafg::LUserInput::GetTriggeredKeys() const
{
    TdhArray<LRawInput> TriggeredKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys())
    {
        if (this->GetLocalEgo()->GetPrimarySurface()->GetLastFramePressedKeys().Contains(Key) == false)
        {
            TriggeredKeys.Emplace(Key);
        }

        continue;
    }

    return TriggeredKeys;
}

Jafg::TdhArray<Jafg::LRawInput>& Jafg::LUserInput::GetOngoingKeys() const
{
    return this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys();
}

Jafg::TdhArray<Jafg::LRawInput> Jafg::LUserInput::GetCompletedKeys() const
{
    TdhArray<LRawInput> CompletedKeys;

    for (const LRawInput& Key : this->GetLocalEgo()->GetPrimarySurface()->GetLastFramePressedKeys())
    {
        if (this->GetLocalEgo()->GetPrimarySurface()->GetCurrentlyPressedKeys().Contains(Key) == false)
        {
            CompletedKeys.Emplace(Key);
        }

        continue;
    }

    return CompletedKeys;
}

Jafg::LUserInputContext* Jafg::LUserInput::GetContextByName(const LSimpleString& InName)
{
    return GetContextByName(GET_NAME(InName));
}

Jafg::LUserInputContext* Jafg::LUserInput::GetCheckedContextByName(const LSimpleString& InName)
{
#if DO_CHECKS
    LUserInputContext* Context = this->GetContextByName(InName);
    check( Context )
    return Context;
#else /* DO_CHECKS */
    return this->GetContextByName(InName);
#endif /* !DO_CHECKS */
}

Jafg::LUserInputContext* Jafg::LUserInput::GetPanickedContextByName(const LSimpleString& InName)
{
    if (LUserInputContext* Context = this->GetContextByName(InName); Context)
    {
        return Context;
    }

    panicMsgf( "Could not find context with name [{}].", InName )
    return nullptr;
}

void Jafg::LUserInput::GetContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const
{
    return this->GetContextByName(GET_NAME(InName), OutContext);
}

void Jafg::LUserInput::GetCheckedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const
{
    this->GetContextByName(InName, OutContext);
    check( OutContext )
    return;
}

void Jafg::LUserInput::GetPanickedContextByName(const LSimpleString& InName, LUserInputContext*& OutContext) const
{
    this->GetContextByName(InName, OutContext);

    if (OutContext == nullptr)
    {
        panicMsgf( "Could not find context with name [{}].", InName )
    }

    return;
}

Jafg::LUserInputContext* Jafg::LUserInput::GetContextByName(const LName InName)
{
    for (LUserInputContext* Context : this->RegisteredContexts)
    {
        if (Context->GetUniqueIdentifier() == InName)
        {
            return Context;
        }

        continue;
    }

    return nullptr;
}

Jafg::LUserInputContext* Jafg::LUserInput::GetCheckedContextByName(const LName InName)
{
#if DO_CHECKS
    LUserInputContext* Context = this->GetContextByName(InName);
    check( Context )
    return Context;
#else /* DO_CHECKS */
    return this->GetContextByName(InName);
#endif /* !DO_CHECKS */
}

Jafg::LUserInputContext* Jafg::LUserInput::GetPanickedContextByName(const LName InName)
{
    if (LUserInputContext* Context = this->GetContextByName(InName); Context)
    {
        return Context;
    }

    panicMsgf( "Could not find context with name [{}].", InName.ToString() )
    return nullptr;
}

void Jafg::LUserInput::GetContextByName(const LName InName, LUserInputContext*& OutContext) const
{
    for (LUserInputContext* Context : this->RegisteredContexts)
    {
        if (Context->GetUniqueIdentifier() == InName)
        {
            OutContext = Context;
            return;
        }

        continue;
    }

    OutContext = nullptr;
    return;
}

void Jafg::LUserInput::GetCheckedContextByName(const LName InName, LUserInputContext*& OutContext) const
{
    this->GetContextByName(InName, OutContext);
    check( OutContext )
    return;
}

void Jafg::LUserInput::GetPanickedContextByName(const LName InName, LUserInputContext*& OutContext) const
{
    this->GetContextByName(InName, OutContext);

    if (OutContext == nullptr)
    {
        panicMsgf( "Could not find context with name [{}].", InName.ToString() )
    }

    return;
}

Jafg::LInputAction* Jafg::LUserInput::RegisterAction(LInputAction&& InAction)
{
    LInputAction* Action = new LInputAction(std::move(InAction));
    this->RegisteredActions.Add(Action);
    return Action;
}

void Jafg::LUserInput::ActivateContext(const LSimpleString& InName)
{
    this->ActivateContext(this->GetPanickedContextByName(InName));
    return;
}

void Jafg::LUserInput::ActivateContext(LUserInputContext* InContext)
{
    check( InContext )
    this->ActiveContexts.Add(InContext);
    LOG_VERBOSE(LogUserInput, "Activating context [{}].", InContext->GetUniqueIdentifier().ToString())
    return;
}

void Jafg::LUserInput::DeactivateContext(const LSimpleString& InName)
{
    this->DeactivateContext(this->GetPanickedContextByName(InName));
}

void Jafg::LUserInput::DeactivateContext(LUserInputContext* InContext)
{
    check( InContext )
    this->ActiveContexts.RemoveOnceChecked(InContext);
    LOG_VERBOSE(LogUserInput, "Deactivating context [{}].", InContext->GetUniqueIdentifier().ToString())
    return;
}

int32 Jafg::LUserInput::DeactivateAllContexts()
{
    const int32 NumDeactivated = this->ActiveContexts.GetSize();
    this->ActiveContexts.Reset(1);
    LOG_VERBOSE(LogUserInput, "Deactivated all {} contexts.", NumDeactivated)
    return NumDeactivated;
}

void Jafg::LUserInput::DispatchInputDelegatesForAction(const LUserInputContext* InContext, const TdhArray<LRawInput>& InRawInputs, LInputMappedAction* InAction)
{
    LInputActionValue Value = InAction->Action->Category;

    for (const LRawInput& RawInput : InRawInputs)
    {
        for (const LInputActionMappedKey& MappedKey : InAction->Action->MappedKeys)
        {
            if (MappedKey.Context != InContext)
            {
                continue;
            }

            if (MappedKey.Key == RawInput.Key)
            {
                LVector Magnitude = LVector(RawInput.Value, 0.0f, 0.0f);

                for (LInputActionMappedKeyModifier* Modifier : MappedKey.Modifiers)
                {
                    Magnitude = Modifier->ApplyModifier(Magnitude);
                }

                Value += Magnitude;

                continue;
            }

            if (MappedKey.Key == EKeys::MouseXY)
            {
                if (RawInput.Key == EKeys::MouseX)
                {
                    Value += LVector(RawInput.Value, 0.0f, 0.0f);
                }
                else if (RawInput.Key == EKeys::MouseY)
                {
                    Value += LVector(0.0f, RawInput.Value, 0.0f);
                }
            }

            continue;
        }

        continue;
    }

    if (Value.IsNonZero())
    {
        InAction->Callback(Value);
    }

    return;
}
