// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/InputActionTrigger.h"
#include "User/Input/InputActionModifiers.h"
#include "User/Input/InputAction.h"

namespace Jafg
{

class LViewport;
class LUserInput;
class LUserInputRegistry;
struct LInputAction;
struct LInputActionValue;
struct LInputMappedAction;
struct LUserInputContext;

//#
//# @param Value    The value of the action that was triggered.
//# @param Viewport The viewport on which the action was triggered.
//#
typedef TFunction<void(LViewport& Viewport, LInputActionValue& Value)> LOnUserInputAction;

//#
//# A mapped action that is owned by a context.
//#
struct LInputMappedAction
{
    struct LTrigger
    {
        constexpr LTrigger() noexcept = default;
        constexpr LTrigger(TArray<LKey>&& InDefaultKeys, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers = {}) noexcept
            : Keys(std::move(InDefaultKeys)), Type(InType), Modifiers(std::move(InModifiers)) { }
        constexpr LTrigger(const LKey InDefaultKey, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers = {}) noexcept
            : Keys(), Type(InType), Modifiers(std::move(InModifiers)) { this->Keys.emplace_back(InDefaultKey); }
        constexpr LTrigger(LString&& InName, TArray<LKey>&& InDefaultKeys, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers = {}) noexcept
            : Name(std::move(InName)), Keys(std::move(InDefaultKeys)), Type(InType), Modifiers(std::move(InModifiers)) { }
        constexpr LTrigger(LString&& InName, const LKey InDefaultKey, const EInputActionTrigger::Type InType, TArray<TUnique<LInputActionMappedTriggerModifier>>&& InModifiers = {}) noexcept
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
    explicit LInputMappedAction(LUserInputTag ActionTag) noexcept : ActionTag(ActionTag) { check( this->ActionTag.IsSet() ) }
    DEFAULT_MOVE(LInputMappedAction)
    PROHIBIT_COPY(LInputMappedAction)
    ~LInputMappedAction() = default;

    FORCEINLINE bool operator==(const LInputMappedAction& InOther) const noexcept { return this->ActionTag == InOther.ActionTag; }

    //# The mapped action.
    LUserInputTag ActionTag;

    //#
    //# Through what the action can be triggerd in this context.
    //#
    TArray<LTrigger> Triggers;

    //#
    //# The callback to call if the action was triggered while the owing context is active and valid.
    //#
    LOnUserInputAction Callback;
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
    LUserInputContext() noexcept = delete;
    explicit LUserInputContext(LUserInputTag Name) noexcept : LUserInputContext(Name, algo::add_spaces_to_camel_case(Name.ToString())) {}
    LUserInputContext(LUserInputTag Tag, LString DisplayName) noexcept : Tag(Tag), DisplayName(std::move(DisplayName)) { check( this->Tag.IsSet() ) }
    explicit LUserInputContext(LString const& DisplayName) noexcept : LUserInputContext(LUserInputTag::ToTag(DisplayName), DisplayName) {}
    DEFAULT_REALLOC_OF_ANY_FORM(LUserInputContext)
    ~LUserInputContext() = default;

    //#
    //# Registers the action to the active #LUserInputRegistry and maps to this context it.
    //# @return The newly mapped action.
    //#
    ENGINE_API LInputMappedAction* MapAction(LUserInputRegistry* Registry, LInputAction&& TransientAction) noexceptcheck;

    //#
    //# Map an already registered (inside the user input registry) action.
    //# @return The newly mapped action.
    //#
    ENGINE_API LInputMappedAction* MapAction(LUserInputTag ActionTag) noexceptcheck;

    FORCEINLINE bool operator==(LUserInputContext const& Other) const noexcept { return this->Tag == Other.Tag; }

    FORCEINLINE const LUserInputTag& GetTag() const noexcept { return this->Tag; }
    FORCEINLINE const LString& GetDisplayName() const noexcept{ return this->DisplayName; }

    FORCEINLINE LInputMappedAction* FindMappedAction(LUserInputTag Tag) noexcept { return algo::wfind_pointer(this->MappedActions, Tag, &LInputMappedAction::ActionTag); }
    FORCEINLINE LInputMappedAction const* FindMappedAction(LUserInputTag Tag) const noexcept { return algo::wfind_pointer(this->MappedActions, Tag, &LInputMappedAction::ActionTag); }
    FORCEINLINE LInputMappedAction* FindMappedActionChecked(LUserInputTag Tag) noexcept { auto* Out{ this->FindMappedAction(Tag) }; check( Out ) return Out; }
    FORCEINLINE LInputMappedAction const* FindMappedActionChecked(LUserInputTag Tag) const noexcept { auto* Out{ this->FindMappedAction(Tag) }; check( Out ) return Out; }
    FORCEINLINE LInputMappedAction* FindMappedActionAsserted(LUserInputTag Tag) noexcept { auto* Out{ this->FindMappedAction(Tag) }; jassert( Out ) return Out; }
    FORCEINLINE LInputMappedAction const* FindMappedActionAsserted(LUserInputTag Tag) const noexcept { auto* Out{ this->FindMappedAction(Tag) }; jassert( Out ) return Out; }

    FORCEINLINE TArray<LInputMappedAction> const& GetMappedActions() const noexcept { return this->MappedActions; }

    ///////////////////////////////////////////////////////////////////////////////
    // Helper methods for faster and less boilerplate action registration.
#pragma region "Helper methods for faster and less boilerplate action registration."

    ENGINE_API LInputMappedAction* MapAction
    (
        LUserInputRegistry* Registry,
        LInputAction&& TransientAction,
        LString TriggerName,
        const LKey DefaultKey,
        const EInputActionTrigger::Type ActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& Modifiers,
        LOnUserInputAction&& Callback
    ) noexcept;

    FORCEINLINE LInputMappedAction* MapAction
    (
        LUserInputTag ActionTag,
        LString TriggerName,
        const LKey DefaultKey,
        const EInputActionTrigger::Type ActionTrigger,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& Modifiers,
        LOnUserInputAction&& Callback
    ) noexcept
    {
        LInputMappedAction* MappedAction{ this->MapAction(ActionTag) };
        check( MappedAction )
        MappedAction->Triggers.emplace_back(std::move(TriggerName), DefaultKey, ActionTrigger, std::move(Modifiers));
        MappedAction->Callback = std::move(Callback);

        return MappedAction;
    }

    ENGINE_API LInputMappedAction* MapAction
    (
        LUserInputRegistry* Registry,
        LInputAction&& TransientAction,
        TArray<LInputMappedAction::LTrigger>&& Triggers,
        LOnUserInputAction&& Callback
    ) noexcept;

    FORCEINLINE LInputMappedAction* MapAction
    (
        LUserInputTag ActionTag,
        TArray<LInputMappedAction::LTrigger>&& Triggers,
        LOnUserInputAction&& Callback
    ) noexcept
    {
        LInputMappedAction* MappedAction{ this->MapAction(ActionTag) };
        check( MappedAction )
        MappedAction->Triggers = std::move(Triggers);
        MappedAction->Callback = std::move(Callback);

        return MappedAction;
    }

#pragma endregion "Helper methods for faster and less boilerplate action registration."
    // ~Helper methods for faster and less boilerplate action registration.
    ///////////////////////////////////////////////////////////////////////////////

private:

    LUserInputTag Tag;
    LString DisplayName;
    TArray<LInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
