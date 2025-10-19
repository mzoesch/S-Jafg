// Copyright mzoesch. All rights reserved.

#include "User/Input/InputContext.h"
#include "User/Input/InputAction.h"
#include "User/Input/UserInput.h"
#if LAL_DO_CHECKS
    #include "Engine/Engine.h"
#endif /* WITH_CHECKS */

Jafg::LUserInputContext::LUserInputContext(const LName InUniqueIdentifier)
    : Name(InUniqueIdentifier), DisplayName(Strings::AddSpacesToCamelCase(InUniqueIdentifier.ToString()))
{
    if (this->Name.IsSet() == false)
    {
        panic( "Unique identifier must not be empty." )
    }

    return;
}

Jafg::LUserInputContext::LUserInputContext(LName InUniqueIdentifier, const LString& InDisplayName)
    : Name(std::move(InUniqueIdentifier)), DisplayName(InDisplayName)
{
    if (this->Name.IsSet() == false)
    {
        panic( "Unique identifier must not be empty." )
    }

    return;
}

Jafg::LUserInputContext::LUserInputContext(const LString& InDisplayName)
    : Name(MAKE_NAME(InDisplayName)), DisplayName(InDisplayName)
{
    check( this->Name.IsSet() )

    return;
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(LUserInput* InUserInput, LInputAction&& InAction)
{
    check( InUserInput )
    return this->MapAction(InUserInput->RegisterAction(std::move(InAction)));
}

Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction(const LInputAction* InAction)
{
    check( InAction )

    checkCode
    (
        const LInputAction* Action = GEngine->GetLocalEgo().GetUserInput().GetActionByName(InAction->GetName());
        check( Action )
        check( *Action == *InAction )
    )

    if (LInputMappedAction* Out = this->FindMappedAction(InAction); Out)
    {
        LOG_WARNING(LogUserInput, "Action was already mapped." )
        return Out;
    }

    this->MappedActions.emplace_back(InAction);

    return &this->MappedActions.back();
}


Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction
(
    LUserInput* InUserInput,
    LInputAction&& InAction,
    LString&& InName,
    const LKey InDefaultKey,
    const EInputActionTrigger::Type InActionTrigger,
    TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
    LUserInputActionCallback&& InCallback
)
{
    return this->MapAction(InUserInput->RegisterAction(std::move(InAction)), std::move(InName), InDefaultKey, InActionTrigger, std::move(InModifiers), std::move(InCallback));
}


Jafg::LInputMappedAction* Jafg::LUserInputContext::MapAction
(
    LUserInput* InUserInput,
    LInputAction&& InAction,
    TArray<LInputMappedAction::LTrigger>&& InTriggers,
    LUserInputActionCallback&& InCallback
)
{
    return this->MapAction(InUserInput->RegisterAction(std::move(InAction)), std::move(InTriggers), std::move(InCallback));
}
