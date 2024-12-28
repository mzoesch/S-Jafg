// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/InputContext.h"
#include "User/Input/InputAction.h"
#include "User/Input/UserInput.h"

Jafg::LUserInputContext::LUserInputContext(const LName InUniqueIdentifier) : UniqueIdentifier(InUniqueIdentifier)
{
    if (InUniqueIdentifier.IsSet() == false)
    {
        panic( "Unique identifier must not be empty." )
    }

    return;
}

Jafg::LInputAction* Jafg::LUserInputContext::MapAction(LInputAction&& InAction, LUserInput* InUserInput)
{
    LInputAction* RegisteredAction = InUserInput->RegisterAction(std::move(InAction));
    this->MappedActions.Emplace(RegisteredAction);
    check( this->MappedActions.GetLast()->Action == RegisteredAction )
    return RegisteredAction;
}

void Jafg::LUserInputContext::MapAction(LInputAction* InAction)
{
    check( InAction )

    if (this->FindMappedAction(InAction))
    {
        LOG_WARNING(LogUserInput, "Action was already mapped." )
        return;
    }

    this->MappedActions.Emplace(InAction);

    return;
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(LInputAction* InAction, const LKey InKey)
{
    check( InAction )

    const LInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);
    MappedAction->Action->MappedKeys.Emplace(this, InKey);

    return MappedAction->Action->MappedKeys.GetLast();
}

Jafg::LInputActionMappedKey* Jafg::LUserInputContext::MapKey(
    LInputAction* InAction,
    const LKey InKey,
    const EInputActionTrigger::Type InTrigger,
    LUserInputActionCallback&& InCallback
)
{
    this->MapCallback(InAction, InTrigger, std::move(InCallback));
    return this->MapKey(InAction, InKey);
}

void Jafg::LUserInputContext::MapCallback(const LInputAction* InAction, const EInputActionTrigger::Type InTrigger, LUserInputActionCallback&& InCallback)
{
    check( InAction )
    check( InTrigger != EInputActionTrigger::None )
    check( InCallback.IsBound() )

    LInputMappedAction* MappedAction = this->FindCheckedMappedAction(InAction);

    if constexpr (IS_COMPILED_LOG(LogUserInput, Warning))
    {
        if (MappedAction->Callback)
        {
            LOG_WARNING(LogUserInput, "Callback was already mapped." )
        }
    }

    MappedAction->Callback = std::move(InCallback);
    MappedAction->Trigger  = InTrigger;

    check( MappedAction->Callback.IsBound() )

    return;
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
