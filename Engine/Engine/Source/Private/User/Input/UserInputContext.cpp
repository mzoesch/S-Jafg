// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "User/Input/UserInputContext.h"

Jafg::LUserInputContext::LUserInputContext(const LSimpleString& InUniqueIdentifier)
    : UniqueIdentifier(InUniqueIdentifier)
{
    if (InUniqueIdentifier.IsEmpty())
    {
        panic( "Unique identifier must not be empty." )
    }

    return;
}

void Jafg::LUserInputContext::MapAction(
    LUserInputAction& InAction,
    const LKey InKey,
    const EUserInputActionTrigger::Type InTrigger,
    const LUserInputActionCallback&& InCallback
)
{
    if (this->IsActionWithSameKeyAlreadyMapped(InAction, InKey))
    {
#if !IN_SHIPPING
        LOG_WARNING(
            LogUserInput,
            "Action with key [{}] is already mapped in context %s. Ignoring mapping request.",
            EKeys::ToString(InKey),
            this->UniqueIdentifier
        )
#else /* !IN_SHIPPING */
        panicMsgf(
            "Action with key [{}] is already mapped in context %s.",
            EKeys::ToString(Key),
            this->UniqueIdentifier
        )
#endif /* IN_SHIPPING */
        return;
    }

    if (InCallback.IsSet() == false)
    {
        panic( "Callback must be set." )
        return;
    }

    this->MappedActions.Add(LUserInputMappedAction(&InAction, InKey, InTrigger, InCallback));

    return;
}

bool Jafg::LUserInputContext::IsActionWithSameKeyAlreadyMapped(const LUserInputAction& Action, const LKey Key) const
{
    for (const LUserInputMappedAction& MappedAction : this->MappedActions)
    {
        if (MappedAction.Action == &Action && MappedAction.Key == Key)
        {
            return true;
        }
    }

    return false;
}
