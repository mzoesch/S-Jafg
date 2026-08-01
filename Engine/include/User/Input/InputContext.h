// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"
#include "User/Input/InputActionTrigger.h"
#include "User/Input/InputActionModifiers.h"
#include "User/Input/InputAction.h"
#include "User/Input/InputCallback.h"

namespace Jafg
{

class LViewport;
class LUserInput;
class LUserInputRegistry;
struct LInputAction;
struct LInputActionValue;
struct LInputMappedAction;
struct LUserInputContext;

struct LOnUserInputActionResult final
{
    //# Whether active contexts changed.
    bool bDirty{};
};

typedef TFunction<LOnUserInputActionResult(LInputCallback const& Data, LInputActionValue& Value)> LOnUserInputAction;

//#
//# A trigger for a mapped action on a context.
//# This trigger is usually tightly coupled to a physical / virtual key.
//#
struct LInputTrigger final
{
    // LInputTrigger(LString Name, EKey Key, EInputActionTriggerFlags TriggerFlags, TArray<TUnique<LInputActionMappedTriggerModifier>> Modifiers) noexcept
    //     : Name(std::move(Name)), Keys{TArray<EKey>{Key}}, TriggerFlags(TriggerFlags), Modifiers(std::move(Modifiers)) {}
    // LInputTrigger(LString Name, TArray<EKey> Keys, EInputActionTriggerFlags TriggerFlags, TArray<TUnique<LInputActionMappedTriggerModifier>> Modifiers) noexcept
    //     : Name(std::move(Name)), Keys(std::move(Keys)), TriggerFlags(TriggerFlags), Modifiers(std::move(Modifiers)) {}
    // PROHIBIT_COPY(LInputTrigger)
    // DEFAULT_MOVE(LInputTrigger)

    LString Name;
    TArray<LPhysicalKey> Keys;
    EInputActionTriggerFlags TriggerFlags;
    TArray<TUnique<LInputActionMappedTriggerModifier>> Modifiers;
};

//# A mapped action that is owned by a context.
struct LInputMappedAction final
{
    LInputMappedAction() = delete;
    explicit LInputMappedAction(LUserInputTag ActionTag) noexcept : ActionTag{ActionTag} { check(this->ActionTag.IsSet()) }
    DEFAULT_MOVE(LInputMappedAction)
    PROHIBIT_COPY(LInputMappedAction)
    ~LInputMappedAction() noexcept = default;

    FORCEINLINE bool operator==(LInputMappedAction const& InOther) const noexcept { return this->ActionTag == InOther.ActionTag; }

    //# The mapped action.
    LUserInputTag ActionTag;
    //# Through what the action can be triggerd in this context.
    TArray<LInputTrigger> Triggers;
    //# The callback to call if the action was triggered while the owing context is active and valid.
    LOnUserInputAction Callback;
};

//#
//# A context that can be used to have a set of actions that are mapped to keys and callbacks.
//#
struct LUserInputContext final
{
    LUserInputContext() noexcept = delete;
    LUserInputContext(LUserInputTag Tag, LString DisplayName) noexcept : Tag(Tag), DisplayName(std::move(DisplayName)) { check(this->Tag.IsSet()) }
    LUserInputContext(LUserInputTag Tag) noexcept : LUserInputContext{Tag, algo::add_spaces_to_camel_case(Tag.ToString())} {}
    explicit LUserInputContext(LString const& DisplayName) noexcept : LUserInputContext{LUserInputTag::ToTag(DisplayName), DisplayName} {}
    explicit LUserInputContext(char const* DisplayName) noexcept : LUserInputContext{LUserInputTag::ToTag(DisplayName), DisplayName} {}
    DEFAULT_REALLOC_OF_ANY_FORM(LUserInputContext)
    ~LUserInputContext() noexcept = default;

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

    FORCEINLINE LInputMappedAction* FindMappedAction(LUserInputTag Tag) noexcept { return algo::find_pointer(this->MappedActions, Tag, &LInputMappedAction::ActionTag); }
    FORCEINLINE LInputMappedAction const* FindMappedAction(LUserInputTag Tag) const noexcept { return algo::find_pointer(this->MappedActions, Tag, &LInputMappedAction::ActionTag); }
    FORCEINLINE LInputMappedAction* FindMappedActionChecked(LUserInputTag Tag) noexcept { auto* Out{ this->FindMappedAction(Tag) }; check( Out ) return Out; }
    FORCEINLINE LInputMappedAction const* FindMappedActionChecked(LUserInputTag Tag) const noexcept { auto* Out{ this->FindMappedAction(Tag) }; check( Out ) return Out; }
    FORCEINLINE LInputMappedAction* FindMappedActionAsserted(LUserInputTag Tag) noexcept { auto* Out{ this->FindMappedAction(Tag) }; jassert( Out ) return Out; }
    FORCEINLINE LInputMappedAction const* FindMappedActionAsserted(LUserInputTag Tag) const noexcept { auto* Out{ this->FindMappedAction(Tag) }; jassert( Out ) return Out; }

    FORCEINLINE TArray<LInputMappedAction> const& GetMappedActions() const noexcept { return this->MappedActions; }

    ENGINE_API LInputMappedAction* MapAction(
        LUserInputRegistry* Registry,
        LInputAction&& TransientAction,
        LPhysicalKey DefaultKey,
        EInputActionTriggerFlags TriggerFlags,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& Modifiers,
        LOnUserInputAction&& Callback
        ) noexcept;
    FORCEINLINE LInputMappedAction* MapAction(
        LUserInputTag ActionTag,
        LPhysicalKey Key,
        EInputActionTriggerFlags TriggerFlags,
        TArray<TUnique<LInputActionMappedTriggerModifier>>&& Modifiers,
        LOnUserInputAction&& Callback
        ) noexcept
    {
        LInputMappedAction* MappedAction{this->MapAction(ActionTag)};
        check(MappedAction)
        MappedAction->Triggers.emplace_back(LString{}, TArray<LPhysicalKey>{Key}, TriggerFlags, std::move(Modifiers));
        MappedAction->Callback = std::move(Callback);
        return MappedAction;
    }

    ENGINE_API LInputMappedAction* MapAction(
        LUserInputRegistry* Registry,
        LInputAction&& TransientAction,
        TArray<LInputTrigger>&& Triggers,
        LOnUserInputAction&& Callback
        ) noexcept;
    FORCEINLINE LInputMappedAction* MapAction(
        LUserInputTag ActionTag,
        TArray<LInputTrigger>&& Triggers,
        LOnUserInputAction&& Callback
        ) noexcept
    {
        LInputMappedAction* MappedAction{ this->MapAction(ActionTag) };
        check(MappedAction)
        MappedAction->Triggers = std::move(Triggers);
        MappedAction->Callback = std::move(Callback);
        return MappedAction;
    }

private:

    LUserInputTag Tag;
    LString DisplayName;
    TArray<LInputMappedAction> MappedActions;
};

} /* ~Namespace Jafg */
