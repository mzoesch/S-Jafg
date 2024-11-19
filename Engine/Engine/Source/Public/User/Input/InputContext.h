// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputTypes.h"
#include "User/Input/InputActionTrigger.h"

namespace Jafg
{

struct LInputAction;
struct LInputActionValue;
struct LInputActionMappedKey;

typedef TFunction<void(LInputActionValue& InValue)> LUserInputActionCallback;

MAKE_MULTICAST_SIGNATURE(OnActionTriggered, const LInputActionValue&)

/**
 * A mapped action that is owned by a context.
 */
struct LInputMappedAction
{
    LInputMappedAction() = delete;
    explicit LInputMappedAction(LInputAction* InAction) : Action(InAction) { check( InAction ) }
    DEFAULT_REALLOC_OF_ANY_FORM(LInputMappedAction)
    ~LInputMappedAction() = default;

    void ResetCallback();
    void SetCallback(const EInputActionTrigger::Type InTrigger, const LUserInputActionCallback& InCallback);

    LInputAction*             Action;
    EInputActionTrigger::Type Trigger  = EInputActionTrigger::None;
    LUserInputActionCallback* Callback = nullptr;
};

/**
 * A context that can be used to have a set of actions that are mapped to keys and callbacks.
 */
struct ENGINE_API LUserInputContext final
{
    LUserInputContext() = delete;
    explicit LUserInputContext(const LSimpleString& InUniqueIdentifier);
    DEFAULT_REALLOC_OF_ANY_FORM(LUserInputContext)
    ~LUserInputContext() = default;

    /** @return The newly mapped action. This is not the same as the input argument. */
    LInputAction* MapAction(const LInputAction& InAction);

    auto MapKey(LInputAction* InAction, const LKey InKey) -> LInputActionMappedKey*;
    auto MapCallback(
        const LInputAction* InAction,
        const EInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback& InCallback
    ) -> void;
    auto MapKey(
        LInputAction* InAction,
        const LKey InKey,
        const EInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback& InCallback
    ) -> LInputActionMappedKey*;

    FORCEINLINE auto GetUniqueIdentifier() const -> const LSimpleString& { return this->UniqueIdentifier; }
    FORCEINLINE auto GetMappedActions()       ->       TdhArray<LInputMappedAction>& { return this->MappedActions; }
    FORCEINLINE auto GetMappedActions() const -> const TdhArray<LInputMappedAction>& { return this->MappedActions; }

    auto FindMappedAction(const LInputAction* InAction) -> LInputMappedAction*;
    auto FindCheckedMappedAction(const LInputAction* InAction) -> LInputMappedAction*;

private:

    LSimpleString UniqueIdentifier;
    TdhArray<LInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
