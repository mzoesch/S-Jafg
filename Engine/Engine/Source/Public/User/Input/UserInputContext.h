// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

#include "User/InputTypes.h"
#include "User/Input/UserInputActionTrigger.h"

namespace Jafg
{

struct LUserInputAction;
struct LUserInputActionValue;

typedef TFunction<void(LUserInputActionValue& InValue)> LUserInputActionCallback;

MAKE_MULTICAST_SIGNATURE(OnActionTriggered, const LUserInputActionValue&)

struct LUserInputMappedAction
{
    LUserInputMappedAction() = delete;
    LUserInputMappedAction(
        LUserInputAction*                   InAction,
        const LKey                          InKey,
        const EUserInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback&     InCallback
    )
        : Action(InAction), Key(InKey), Trigger(InTrigger)
    {
        this->Callback = InCallback;

        if (this->Action == nullptr)
        {
            panic( "Action must not be null." )
        }

        if (this->Callback.IsSet() == false)
        {
            panic( "Callback must be set." )
        }

        return;
    }
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputMappedAction)
    ~LUserInputMappedAction() = default;

    LUserInputAction*             Action;
    LKey                          Key;
    EUserInputActionTrigger::Type Trigger;
    LUserInputActionCallback      Callback;
};

/**
 * A context that can be used to map actions to keys.
 */
struct ENGINE_API LUserInputContext final
{
    LUserInputContext() = delete;
    explicit LUserInputContext(const LSimpleString& InUniqueIdentifier);
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputContext)
    ~LUserInputContext() = default;

    /** This class takes care of the ownership of InAction. */
    void MapAction(
        LUserInputAction& InAction,
        const LKey InKey,
        const EUserInputActionTrigger::Type InTrigger,
        const LUserInputActionCallback&& InCallback
    );

    FORCEINLINE auto GetUniqueIdentifier() const -> const LSimpleString& { return this->UniqueIdentifier; }
    FORCEINLINE auto GetMappedActions()       ->       TdhArray<LUserInputMappedAction>& { return this->MappedActions; }
    FORCEINLINE auto GetMappedActions() const -> const TdhArray<LUserInputMappedAction>& { return this->MappedActions; }

private:

    bool IsActionWithSameKeyAlreadyMapped(const LUserInputAction& Action, const LKey Key) const;

    LSimpleString UniqueIdentifier;
    TdhArray<LUserInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
