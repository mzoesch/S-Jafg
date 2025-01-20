// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionTrigger.h"
#include "Core/Name.h"

namespace Jafg
{

class LUserInput;
struct LInputAction;
struct LInputActionValue;
struct LInputActionMappedKey;

typedef TFunction<void(LInputActionValue& InValue)> LUserInputActionCallback;
MAKE_DELEGATE_SIGNATURE(LUserInputActionDelegate, void, LInputActionValue& InValue)
MAKE_MULTICAST_SIGNATURE(OnActionTriggered, const LInputActionValue&)

//#
//# A mapped action that is owned by a context.
//#
struct LInputMappedAction
{
    LInputMappedAction() = delete;
    explicit LInputMappedAction(LInputAction* InAction) : Action(InAction) { check( InAction ) }
    DEFAULT_REALLOC_OF_ANY_FORM(LInputMappedAction)
    ~LInputMappedAction() = default;

    LInputAction*             Action;
    EInputActionTrigger::Type Trigger  = EInputActionTrigger::None;
    LUserInputActionDelegate  Callback = nullptr;
};

//#
//# A context that can be used to have a set of actions that are mapped to keys and callbacks.
//#
struct ENGINE_API LUserInputContext final
{
    LUserInputContext() = delete;
    explicit LUserInputContext(const LName InUniqueIdentifier);
    DEFAULT_REALLOC_OF_ANY_FORM(LUserInputContext)
    ~LUserInputContext() = default;

    //# @return The newly mapped action. This is not the same as the input argument.
    auto MapAction(LInputAction&& InAction, LUserInput* InUserInput) -> LInputAction*;
    //# Map an already registered (inside the user input) action.
    void MapAction(LInputAction* InAction);

    auto MapKey(LInputAction* InAction, const LKey InKey) -> LInputActionMappedKey*;
    auto MapKey(LInputAction* InAction, const LKey InKey, const EInputActionTrigger::Type InTrigger, LUserInputActionCallback&& InCallback) -> LInputActionMappedKey*;
    template <typename ObjTy, typename CallableTy>
    auto MapKey(LInputAction* InAction, const LKey InKey, const EInputActionTrigger::Type InTrigger, ObjTy* InObject, CallableTy InMember) -> LInputActionMappedKey*
    {
        return this->MapKey(InAction, InKey, InTrigger, LUserInputActionCallback(InObject, InMember));
    }

    void MapCallback(const LInputAction* InAction, const EInputActionTrigger::Type InTrigger, LUserInputActionCallback&& InCallback);
    template <typename ObjTy, typename CallableTy>
    void MapCallback(const LInputAction* InAction, const EInputActionTrigger::Type InTrigger, ObjTy* InObject, CallableTy InMember)
    {
        this->MapCallback(InAction, InTrigger, LUserInputActionCallback(InObject, InMember));
    }

    FORCEINLINE auto GetUniqueIdentifier() const -> const LName& { return this->UniqueIdentifier; }
    FORCEINLINE auto GetMappedActions()       ->       TdhArray<LInputMappedAction>& { return this->MappedActions; }
    FORCEINLINE auto GetMappedActions() const -> const TdhArray<LInputMappedAction>& { return this->MappedActions; }

    auto FindMappedAction(const LInputAction* InAction) -> LInputMappedAction*;
    auto FindCheckedMappedAction(const LInputAction* InAction) -> LInputMappedAction*;

private:

    LName UniqueIdentifier;
    TdhArray<LInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
