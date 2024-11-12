// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/UserInputContext.h"
#include "User/Input/UserInputAction.h"

void Jafg::LUserInputMappedAction::ResetCallback()
{
    this->Callback.Reset();
    this->Trigger = EUserInputActionTrigger::None;

    return;
}

void Jafg::LUserInputMappedAction::SetCallback(
    const EUserInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    if (InTrigger == EUserInputActionTrigger::None)
    {
        panic( "Trigger must not be None." )
        return;
    }

    if (this->Callback.IsSet())
    {
        panic( "Callback must not be set." )
        return;
    }

    this->Trigger  = InTrigger;
    this->Callback = InCallback;

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

Jafg::LUserInputAction* Jafg::LUserInputContext::MapAction(const LUserInputAction& InAction)
{
    LUserInputAction* Action = new LUserInputAction(InAction);
    this->MappedActions.Add(LUserInputMappedAction(Action));
    const LUserInputMappedAction* Last = this->MappedActions.GetLast();
    check( Last->Action == Action )
    return Last->Action;
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(LUserInputAction* InAction, const LKey InKey)
{
    check( InAction )

    const LUserInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);
    MappedAction->Action->MappedKeys.Emplace(InKey);

    return MappedAction->Action->MappedKeys.GetLast();
}

void Jafg::LUserInputContext::MapCallback(
    const LUserInputAction* InAction,
    const EUserInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    check( InAction )
    check( InTrigger != EUserInputActionTrigger::None )
    check( InCallback.IsSet() )

    LUserInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);
    check( MappedAction->Callback.IsSet() == false )

    MappedAction->Callback = InCallback;
    MappedAction->Trigger  = InTrigger;

    return;
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(
    LUserInputAction* InAction,
    const LKey InKey,
    const EUserInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback& InCallback
)
{
    this->MapCallback(InAction, InTrigger, InCallback);
    return this->MapKey(InAction, InKey);
}

Jafg::LUserInputMappedAction* Jafg::LUserInputContext::FindMappedAction(const LUserInputAction* InAction)
{
    check( InAction )

    for (LUserInputMappedAction& MappedAction : this->MappedActions)
    {
        if (MappedAction.Action == InAction)
        {
            return &MappedAction;
        }
    }

    return nullptr;
}

Jafg::LUserInputMappedAction* Jafg::LUserInputContext::FindCheckedMappedAction(const LUserInputAction* InAction)
{
    LUserInputMappedAction* Action = this->FindMappedAction(InAction);
    check( Action )
    return Action;
}
