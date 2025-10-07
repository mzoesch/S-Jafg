// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/InputActionTrigger.h"
#include "User/Input/InputActionModifiers.h"
#include "User/Input/InputAction.h"
#include "Core/Name.h"

namespace Jafg
{

class LUserInput;
struct LInputAction;
struct LInputActionValue;
struct LInputMappedAction;
struct LUserInputContext;

typedef TFunction<void(LInputActionValue& InValue)> LUserInputActionCallback;
MAKE_DELEGATE_SIGNATURE(LUserInputActionDelegate, void, LInputActionValue& InValue)

//#
//# A mapped action that is owned by a context.
//#
struct LInputMappedAction
{
    struct LTrigger
    {
        LTrigger() = default;
        LTrigger(LString&& InName, TArray<LKey>&& InDefaultKeys, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers)
            : Name(std::move(InName)), Keys(std::move(InDefaultKeys)), Type(InType), Modifiers(std::move(InModifiers)) { }
        LTrigger(LString&& InName, const LKey InDefaultKey, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers)
            : Name(std::move(InName)), Keys(), Type(InType), Modifiers(std::move(InModifiers)) { this->Keys.emplace_back(InDefaultKey); }

        PROHIBIT_COPY(LTrigger)
        DEFAULT_MOVE(LTrigger)
        ~LTrigger() = default;

        LString Name;
        TArray<LKey> Keys;
        EInputActionTrigger::Type Type;
        TArray<TUnique<LInputActionMappedTriggerModifier>> Modifiers;
    };

    LInputMappedAction() = delete;
    explicit LInputMappedAction(const LInputAction* InAction) noexcept : Action(InAction) { check( InAction ) }
    DEFAULT_MOVE(LInputMappedAction)
    PROHIBIT_COPY(LInputMappedAction)
    ~LInputMappedAction() = default;

    FORCEINLINE bool operator==(const LInputAction* InOther) const noexcept { return this->Action == InOther; }
    FORCEINLINE bool operator!=(const LInputAction* InOther) const noexcept { return !(*this == InOther); }
    FORCEINLINE bool operator==(const LInputMappedAction& InOther) const noexcept { return this->Action == InOther.Action; }
    FORCEINLINE bool operator!=(const LInputMappedAction& InOther) const noexcept { return !(*this == InOther); }
    FORCEINLINE bool operator==(const LName& InName) const noexcept { return this->Action && this->Action->GetName() == InName; }
    FORCEINLINE bool operator!=(const LName& InName) const noexcept { return !(*this == InName); }

    //#
    //# The mapped action.
    //#
    const LInputAction* Action;

    //#
    //# Through what the action can be triggerd in this context.
    //#
    TArray<LTrigger> Triggers;

    //#
    //# The callback to call if the action was triggered while the owing context is active and valid.
    //#
    LUserInputActionDelegate Callback { nullptr };
};

//#
//# A trigger for a mapped action on a context.
//# This trigger is usually tightly coupled to a physical / virtual key.
//#
using LInputTrigger = LInputMappedAction::LTrigger;

//#
//# A context that can be used to have a set of actions that are mapped to keys and callbacks.
//#
struct LUserInputContext final
{
    LUserInputContext() = default;

    ENGINE_API explicit LUserInputContext(const LName InUniqueIdentifier);
    ENGINE_API explicit LUserInputContext(LName InUniqueIdentifier, const LString& InDisplayName);
    ENGINE_API explicit LUserInputContext(const LString& InDisplayName);

    DEFAULT_REALLOC_OF_ANY_FORM(LUserInputContext)

    ~LUserInputContext() = default;

    FORCEINLINE bool IsValid() const noexcept { return this->Name.IsSet(); }

    //#
    //# Registers the action to the active #LUserInput and maps to this context it.
    //# @return The newly mapped action.
    //#
    ENGINE_API LInputMappedAction* MapAction(LUserInput* InUserInput, LInputAction&& InAction);

    //#
    //# Map an already registered (inside the user input) action.
    //# @return The already mapped action.
    //#
    ENGINE_API LInputMappedAction* MapAction(const LInputAction* InAction);

    FORCEINLINE bool operator==(const LName& InName) const noexcept { return this->Name == InName; }
    FORCEINLINE bool operator!=(const LName& InName) const noexcept { return !(*this == InName); }
    FORCEINLINE bool operator==(const LUserInputContext& InContext) const noexcept { return this->Name == InContext.Name; }
    FORCEINLINE bool operator!=(const LUserInputContext& InContext) const noexcept { return !(*this == InContext.Name); }

    FORCEINLINE const LName& GetName() const { return this->Name; }
    FORCEINLINE const LString& GetDisplayName() const { return this->DisplayName; }

    FORCEINLINE LInputMappedAction* FindMappedAction(const LName& InName);
    FORCEINLINE LInputMappedAction* FindMappedActionChecked(const LName& InName);
    FORCEINLINE LInputMappedAction* FindMappedActionAsserted(const LName& InName);
    FORCEINLINE const LInputMappedAction* FindMappedAction(const LName& InName) const;
    FORCEINLINE const LInputMappedAction* FindMappedActionChecked(const LName& InName) const;
    FORCEINLINE const LInputMappedAction* FindMappedActionAsserted(const LName& InName) const;

    FORCEINLINE LInputMappedAction* FindMappedAction(const LInputAction* InAction);
    FORCEINLINE LInputMappedAction* FindMappedActionChecked(const LInputAction* InAction);
    FORCEINLINE LInputMappedAction* FindMappedActionAsserted(const LInputAction* InAction);
    FORCEINLINE const LInputMappedAction* FindMappedAction(const LInputAction* InAction) const;
    FORCEINLINE const LInputMappedAction* FindMappedActionChecked(const LInputAction* InAction) const;
    FORCEINLINE const LInputMappedAction* FindMappedActionAsserted(const LInputAction* InAction) const;

    FORCEINLINE const TArray<LInputMappedAction>& GetMappedActions() const { return this->MappedActions; }

    ///////////////////////////////////////////////////////////////////////////////
    // Helper methods for faster and less boilerplate action registration.
#pragma region "Helper methods for faster and less boilerplate action registration."

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LInputMappedAction* MapAction
    (
        LUserInput* InUserInput,
        LInputAction&& InAction,
        LString&& InName,
        const LKey InDefaultKey,
        const EInputActionTrigger::Type InActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
        ObjTy* InObject,
        CallableTy InMember
    );

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LInputMappedAction* MapAction
    (
        const LInputAction* InAction,
        LString&& InName,
        const LKey InDefaultKey,
        const EInputActionTrigger::Type InActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
        ObjTy* InObject,
        CallableTy InMember
    );

    ENGINE_API LInputMappedAction* MapAction
    (
        LUserInput* InUserInput,
        LInputAction&& InAction,
        LString&& InName,
        const LKey InDefaultKey,
        const EInputActionTrigger::Type InActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
        LUserInputActionCallback&& InCallback
    );

    FORCEINLINE LInputMappedAction* MapAction
    (
        const LInputAction* InAction,
        LString&& InName,
        const LKey InDefaultKey,
        const EInputActionTrigger::Type InActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
        LUserInputActionCallback&& InCallback
    );

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LInputMappedAction* MapAction
    (
        LUserInput* InUserInput,
        LInputAction&& InAction,
        TArray<LInputMappedAction::LTrigger>&& InTriggers,
        ObjTy* InObject,
        CallableTy InMember
    );

    template <typename ObjTy, typename CallableTy>
    FORCEINLINE LInputMappedAction* MapAction
    (
        const LInputAction* InAction,
        TArray<LInputMappedAction::LTrigger>&& InTriggers,
        ObjTy* InObject,
        CallableTy InMember
    );

    ENGINE_API LInputMappedAction* MapAction
    (
        LUserInput* InUserInput,
        LInputAction&& InAction,
        TArray<LInputMappedAction::LTrigger>&& InTriggers,
        LUserInputActionCallback&& InCallback
    );

    FORCEINLINE LInputMappedAction* MapAction
    (
        const LInputAction* InAction,
        TArray<LInputMappedAction::LTrigger>&& InTriggers,
        LUserInputActionCallback&& InCallback
    );

#pragma endregion "Helper methods for faster and less boilerplate action registration."
    // ~Helper methods for faster and less boilerplate action registration.
    ///////////////////////////////////////////////////////////////////////////////

private:

    LName Name;
    LString DisplayName;
    TArray<LInputMappedAction> MappedActions;
};

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedAction(const LName& InName)
{
    return algo::wfind_pointer(this->MappedActions, InName);
}

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedActionChecked(const LName& InName)
{
    LInputMappedAction* Out = this->FindMappedAction(InName);
    check( Out )
    return Out;
}

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedActionAsserted(const LName& InName)
{
    LInputMappedAction* Out = this->FindMappedAction(InName);
    jassert( Out )
    return Out;
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedAction(const LName& InName) const
{
    return algo::wfind_pointer(this->MappedActions, InName);
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedActionChecked(const LName& InName) const
{
    const LInputMappedAction* Out = this->FindMappedAction(InName);
    check( Out )
    return Out;
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedActionAsserted(const LName& InName) const
{
    const LInputMappedAction* Out = this->FindMappedAction(InName);
    jassert( Out )
    return Out;
}

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedAction(const LInputAction* InAction)
{
    check( InAction )
    LInputMappedAction* Out = algo::find_pointer(this->MappedActions, InAction, &LInputMappedAction::Action);
    return Out ? Out : nullptr;
}

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedActionChecked(const LInputAction* InAction)
{
    LInputMappedAction* Out = this->FindMappedAction(InAction);
    check( Out )
    return Out;
}

FORCEINLINE LInputMappedAction* LUserInputContext::FindMappedActionAsserted(const LInputAction* InAction)
{
    LInputMappedAction* Out = this->FindMappedAction(InAction);
    jassert( Out )
    return Out;
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedAction(const LInputAction* InAction) const
{
    check( InAction )
    const LInputMappedAction* Out = algo::find_pointer(this->MappedActions, InAction, &LInputMappedAction::Action);
    return Out ? Out : nullptr;
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedActionChecked(const LInputAction* InAction) const
{
    const LInputMappedAction* Out = this->FindMappedAction(InAction);
    check( Out )
    return Out;
}

FORCEINLINE const LInputMappedAction* LUserInputContext::FindMappedActionAsserted(const LInputAction* InAction) const
{
    const LInputMappedAction* Out = this->FindMappedAction(InAction);
    jassert( Out )
    return Out;
}

template<typename ObjTy, typename CallableTy>
FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    LUserInput* InUserInput,
    LInputAction&& InAction,
    LString&& InName,
    const LKey InDefaultKey,
    const EInputActionTrigger::Type InActionTrigger,
    TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
    ObjTy* InObject,
    CallableTy InMember
)
{
    return this->MapAction(InUserInput, std::move(InAction), std::move(InName), InDefaultKey, InActionTrigger, std::move(InModifiers), LUserInputActionCallback{InObject, InMember});
}

template<typename ObjTy, typename CallableTy>
FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    const LInputAction* InAction,
    LString&& InName,
    const LKey InDefaultKey,
    const EInputActionTrigger::Type InActionTrigger,
    TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
    ObjTy* InObject,
    CallableTy InMember
)
{
    return this->MapAction(InAction, std::move(InName), InDefaultKey, InActionTrigger, std::move(InModifiers), LUserInputActionCallback{InObject, InMember});
}

FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    const LInputAction* InAction,
    LString&& InName,
    const LKey InDefaultKey,
    const EInputActionTrigger::Type InActionTrigger,
    TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers,
    LUserInputActionCallback&& InCallback
)
{
    LInputMappedAction* MappedAction = this->MapAction(InAction);
    check( MappedAction )
    MappedAction->Triggers.emplace_back(std::move(InName), InDefaultKey, InActionTrigger, std::move(InModifiers));
    MappedAction->Callback = std::move(InCallback);

    return MappedAction;
}

template <typename ObjTy, typename CallableTy>
FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    LUserInput* InUserInput,
    LInputAction&& InAction,
    TArray<LInputMappedAction::LTrigger>&& InTriggers,
    ObjTy* InObject,
    CallableTy InMember
)
{
    return this->MapAction(InUserInput, std::move(InAction), std::move(InTriggers), LUserInputActionCallback{InObject, InMember});
}

template <typename ObjTy, typename CallableTy>
FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    const LInputAction* InAction,
    TArray<LInputMappedAction::LTrigger>&& InTriggers,
    ObjTy* InObject,
    CallableTy InMember
)
{
    return this->MapAction(InAction, std::move(InTriggers), LUserInputActionCallback{InObject, InMember});
}

FORCEINLINE LInputMappedAction* LUserInputContext::MapAction
(
    const LInputAction* InAction,
    TArray<LInputMappedAction::LTrigger>&& InTriggers,
    LUserInputActionCallback&& InCallback
)
{
    LInputMappedAction* MappedAction = this->MapAction(InAction);
    check( MappedAction )
    MappedAction->Triggers = std::move(InTriggers);
    MappedAction->Callback = std::move(InCallback);

    return MappedAction;
}

FORCEINLINE bool operator==(const LInputMappedAction* InA, const LName& InB) { return InA && InA->operator==(InB); }
FORCEINLINE bool operator!=(const LInputMappedAction* InA, const LName& InB) { return !(InA == InB); }
FORCEINLINE bool operator==(const LUserInputContext* InA, const LName& InB) { return InA && InA->operator==(InB); }
FORCEINLINE bool operator!=(const LUserInputContext* InA, const LName& InB) { return !(InA == InB); }
FORCEINLINE bool operator==(const LInputMappedAction* InA, const LInputMappedAction& InB) { return InA && InA->operator==(InB); }
FORCEINLINE bool operator!=(const LInputMappedAction* InA, const LInputMappedAction& InB) { return !(InA == InB); }
FORCEINLINE bool operator==(const LUserInputContext* InA, const LUserInputContext& InB) { return InA && InA->operator==(InB); }
FORCEINLINE bool operator!=(const LUserInputContext* InA, const LUserInputContext& InB) { return !(InA == InB); }

} /* ~Namespace Jafg */
