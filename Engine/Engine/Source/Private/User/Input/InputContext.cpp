// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/InputContext.h"
#include "User/Input/InputAction.h"

void Jafg::LInputMappedAction::ResetCallback()
{
    if (this->Callback)
    {
        this->Callback->Reset();
        delete this->Callback;
        this->Callback = nullptr;
    }
    this->Trigger = EInputActionTrigger::None;

    return;
}

void Jafg::LInputMappedAction::SetCallback(
    const EInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    if (InTrigger == EInputActionTrigger::None)
    {
        panic( "Trigger must not be None." )
        return;
    }

    if (this->Callback && this->Callback->IsSet())
    {
        panic( "Callback must not be set." )
        return;
    }

    if (this->Callback == nullptr)
    {
        this->Callback = new LUserInputActionCallback();
    }

    this->Trigger   = InTrigger;
    *this->Callback = InCallback;

    return;
}

Jafg::LUserInputContext::LUserInputContext(const LSimpleString& InUniqueIdentifier)
    : UniqueIdentifier(InUniqueIdentifier)
{
    if (InUniqueIdentifier.IsEmpty())
    {
        panic( "Unique identifier must not be empty." )
    }

    return;
}

Jafg::LInputAction* Jafg::LUserInputContext::MapAction(const LInputAction& InAction)
{
    LInputAction* Action = new LInputAction(InAction);
    this->MappedActions.Add(LInputMappedAction(Action));
    const LInputMappedAction* Last = this->MappedActions.GetLast();
    check( Last->Action == Action )
    return Last->Action;
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(LInputAction* InAction, const LKey InKey)
{
    check( InAction )

    const LInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);
    MappedAction->Action->MappedKeys.Emplace(InKey);

    return MappedAction->Action->MappedKeys.GetLast();
}

void Jafg::LUserInputContext::MapCallback(
    const LInputAction* InAction,
    const EInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    check( InAction )
    check( InTrigger != EInputActionTrigger::None )
    check( InCallback.IsSet() )

    LInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);

    if (MappedAction->Callback)
    {
        LOG_WARNING(LogUserInput, "Callback was already mapped." )
        MappedAction->Callback->Reset();
        delete MappedAction->Callback;
    }

    MappedAction->Callback  = new LUserInputActionCallback();
    *MappedAction->Callback = InCallback;
    MappedAction->Trigger   = InTrigger;

    check( MappedAction->Callback->IsSet() )

    return;
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(
    LInputAction* InAction,
    const LKey InKey,
    const EInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    this->MapCallback(InAction, InTrigger, InCallback);
    return this->MapKey(InAction, InKey);
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::FindMappedAction(const LInputAction* InAction)
{
    check( InAction )

    for (LInputMappedAction& MappedAction : this->MappedActions)
    {
        if (MappedAction.Action == InAction)
        {
            return &MappedAction;
        }
    }

    return nullptr;
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::FindCheckedMappedAction(const LInputAction* InAction)
{
    LInputMappedAction* Action = this->FindMappedAction(InAction);
    check( Action )
    return Action;
}
