// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputTypes.h"
#include "User/Input/UserInputActionTrigger.h"

namespace Jafg
{

struct LUserInputAction;
struct LInputActionValue;
struct LInputActionMappedKey;

typedef TFunction<void(LInputActionValue& InValue)> LUserInputActionCallback;

MAKE_MULTICAST_SIGNATURE(OnActionTriggered, const LInputActionValue&)

struct LUserInputMappedAction
{
    LUserInputMappedAction() = delete;
    explicit LUserInputMappedAction(LUserInputAction* InAction) : Action(InAction) { check( InAction ) }
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputMappedAction)
    ~LUserInputMappedAction() = default;

    void ResetCallback();
    void SetCallback(const EUserInputActionTrigger::Type InTrigger, const LUserInputActionCallback& InCallback);

    LUserInputAction*             Action;
    EUserInputActionTrigger::Type Trigger  = EUserInputActionTrigger::None;
    LUserInputActionCallback      Callback = nullptr;
};

/**
 * A context that can be used to have a set of actions that are mapped to keys and callbacks.
 */
struct ENGINE_API LUserInputContext final
{
    LUserInputContext() = delete;
    explicit LUserInputContext(const LSimpleString& InUniqueIdentifier);
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputContext)
    ~LUserInputContext() = default;

    /** @return The newly mapped action. This is not the same as the input argument. */
    LUserInputAction* MapAction(const LUserInputAction& InAction);

    auto MapKey(LUserInputAction* InAction, const LKey InKey) -> LInputActionMappedKey*;
    auto MapCallback(
        const LUserInputAction* InAction,
        const EUserInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback& InCallback
    ) -> void;
    auto MapKey(
        LUserInputAction* InAction,
        const LKey InKey,
        const EUserInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback& InCallback
    ) -> LInputActionMappedKey*;

    FORCEINLINE auto GetUniqueIdentifier() const -> const LSimpleString& { return this->UniqueIdentifier; }
    FORCEINLINE auto GetMappedActions()       ->       TdhArray<LUserInputMappedAction>& { return this->MappedActions; }
    FORCEINLINE auto GetMappedActions() const -> const TdhArray<LUserInputMappedAction>& { return this->MappedActions; }

    auto FindMappedAction(const LUserInputAction* InAction) -> LUserInputMappedAction*;
    auto FindCheckedMappedAction(const LUserInputAction* InAction) -> LUserInputMappedAction*;

private:

    LSimpleString UniqueIdentifier;
    TdhArray<LUserInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
