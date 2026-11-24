// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputTypes.h"

namespace Jafg
{

class LViewport;
class LUserInput;
class LUserInputRegistry;
struct LInputAction;
struct LInputActionValue;
struct LInputContext;
class APersonaController;

struct LInputCallback final
{
    LViewport& Viewport;
    APersonaController& Controller;
    LUserInput& UserInput;
};

struct LOnUserInputActionResult final
{
    //# Whether active contexts changed.
    bool bDirty{};
};

typedef TFunction<LOnUserInputActionResult(LInputCallback const& Data, LInputActionValue& Value)> LOnUserInputAction;

enum struct EInputActionTriggerBits: u8
{
    Identity = 0x0 << 0,

    //# Marks an action that was just triggered.
    Triggered = 0x1 << 0,
    //# Marks an action that is ongoing.
    //# Will be called the same tick where the action was triggered.
    Ongoing   = 0x1 << 1,
    //# Marks an action that was just completed.
    Completed = 0x1 << 2,
};
SERDE_ENUM(EInputActionTriggerBits, Identity, Triggered, Ongoing, Completed)
ENUM_STRUCT_FLAGS(EInputActionTriggerBits, EInputActionTriggerFlags)

//# Base struct for input action modifiers. Inherit from this to add your own modifiers.
struct LInputActionMappedTriggerModifier
{
    virtual ~LInputActionMappedTriggerModifier() = default;
    NODISCARD virtual LVec3F ApplyModifier(LVec3F const& Value) const noexcept = 0;
};
//#
//# Modifiers are immutable. Therefore, triggers share them, and triggers and actions stay copyable even though their
//# members are const.
//#
typedef std::shared_ptr<const LInputActionMappedTriggerModifier> LInputActionMappedTriggerModifierPtr;
//# Factory function to create a modifier of type TModifier.
template<typename TModifier> requires(!std::is_abstract_v<TModifier> && std::is_base_of_v<LInputActionMappedTriggerModifier, TModifier>)
NODISCARD FORCEINLINE LInputActionMappedTriggerModifierPtr MakeInputModifier() noexcept
{
    return std::make_shared<const TModifier>();
}
typedef TArray<LInputActionMappedTriggerModifierPtr> LInputActionMappedTriggerModifiers;

//# Negate all axes of the input value. */
struct LInputActionMappedKeyNegateModifier final: LInputActionMappedTriggerModifier
{
    NODISCARD virtual LVec3F ApplyModifier(LVec3F const& Value) const noexcept override { return -Value; }
};

//# Swizzles two different axis's.
struct LInputActionMappedKeySwizzleXYModifier final: LInputActionMappedTriggerModifier
{
    NODISCARD virtual LVec3F ApplyModifier(LVec3F const& Value) const noexcept override { return maths::yxz(Value); }
};

//# Swizzles two different axis's.
struct LInputActionMappedKeySwizzleXZModifier final: LInputActionMappedTriggerModifier
{
    NODISCARD virtual LVec3F ApplyModifier(LVec3F const& Value) const noexcept override { return maths::zyx(Value); }
};

//# Swizzles two different axis's.
struct LInputActionMappedKeySwizzleYZModifier final: LInputActionMappedTriggerModifier
{
    NODISCARD virtual LVec3F ApplyModifier(LVec3F const& Value) const noexcept override { return maths::xzy(Value); }
};

//#
//# A trigger for a mapped action on a context.
//# This trigger is usually tightly coupled to a physical / virtual key.
//#
struct LInputTrigger final
{
    struct CreateInfo final
    {
        LString DisplayName;
        EInputActionTriggerFlags TriggerFlags;
        TArray<LKeyChord> DefaultKeys;
        LInputActionMappedTriggerModifiers Modifiers;
    };
    LInputTrigger(CreateInfo Info)
        : DisplayName{std::move(Info.DisplayName)}
        , TriggerFlags{Info.TriggerFlags}
        , Modifiers{std::move(Info.Modifiers)}
        , DefaultKeys{std::move(Info.DefaultKeys)}
    {
        check(this->TriggerFlags != EInputActionTriggerBits::Identity)
    }

    const LString DisplayName;
    //# The keys that trigger this trigger. The user may change them.
    mutable TArray<LKeyChord> Keys;
    const EInputActionTriggerFlags TriggerFlags;
    const LInputActionMappedTriggerModifiers Modifiers;
    //# The keys this trigger was mapped with, before any user overrides were applied. Set when the action is mapped.
    const TArray<LKeyChord> DefaultKeys;
};

enum struct EInputActionBits: u8
{
    Identity = 0x0 << 0,
    //# The user cannot change the keys of this action.
    ReadOnly = 0x1 << 0,
};
ENUM_STRUCT_FLAGS(EInputActionBits, EInputActionFlags)

SERDE_DECLARE_TRIVIAL_ENUM(EInputActionCategory, u8,
    None,
    Boolean,
    Axis1D,
    Axis2D,
    Axis3D
    )

//#
//# A form of an action that can be triggered by the user.
//# This is the logical representation of something the user wants to / can do (e.g., "Close frontend").
//# It is not tied to any specific physical key - but many physical keys can be bound to this action and can be
//# changed modularly based on delegates fired in the application / state of the program.
//#
struct LInputAction final
{
    const LUserInputTag Tag;
    const LString DisplayName;
    const EInputActionFlags Flags;
    const EInputActionCategory Category{ EInputActionCategory::None };
    //# Through what the action can be triggerd in its context.
    const TArray<LInputTrigger> Triggers;
    //# The callback to call if the action was triggered while its owing context is active and valid.
    const LOnUserInputAction Callback;

    NODISCARD FORCEINLINE bool operator==(LInputAction const& Rhs) const noexcept { return this->Tag == Rhs.Tag; }
};

//# A context that can be used to have a set of actions that are mapped to keys and callbacks.
struct LInputContext final
{
    LInputContext() noexcept = delete;
    LInputContext(LUserInputTag Tag, LString DisplayName) noexcept: Tag{Tag}, DisplayName{std::move(DisplayName)}
    {
        check(Tag.IsSet())
    }
    DEFAULT_REALLOC_OF_ANY_FORM(LInputContext)
    ~LInputContext() noexcept = default;

    NODISCARD FORCEINLINE constexpr bool operator==(LInputContext const& Rhs) const noexcept { return this->Tag == Rhs.Tag; }

    NODISCARD FORCEINLINE LUserInputTag const& GetTag() const noexcept { return this->Tag; }
    NODISCARD FORCEINLINE LString const& GetDisplayName() const noexcept{ return this->DisplayName; }

    struct ActionCreateInfo final
    {
        LUserInputTag Tag;
        LString DisplayName;
        EInputActionFlags Flags;
        EInputActionCategory Category;
        TArray<LInputTrigger> Triggers;
        LOnUserInputAction Callback;
    };
    LInputAction const& MapAction(ActionCreateInfo Info)
    {
        check(Tasks::IsOnMasterThread())
        check(Info.Tag.IsSet())
        check(!algo::contains(this->Actions, Info.Tag, &LInputAction::Tag))

        if (Info.DisplayName.empty())
        {
            Info.DisplayName = algo::add_spaces_to_camel_case(Info.Tag.ToString());
        }

        auto& Result{this->Actions.emplace_back(Info.Tag, std::move(Info.DisplayName)
            , Info.Flags
            , Info.Category
            , std::move(Info.Triggers)
            , std::move(Info.Callback)
            )};
        this->OnActionMapped(Result);
        return Result;
    }

    NODISCARD FORCEINLINE LInputAction* FindAction(LUserInputTag Tag) noexcept { return algo::find_pointer(this->Actions, Tag, &LInputAction::Tag); }
    NODISCARD FORCEINLINE LInputAction const* FindAction(LUserInputTag Tag) const noexcept { return algo::find_pointer(this->Actions, Tag, &LInputAction::Tag); }
    NODISCARD FORCEINLINE LInputAction& GetAction(LUserInputTag Tag) noexcept { auto* Result{algo::find_pointer(this->Actions, Tag, &LInputAction::Tag)}; check(Result) return *Result; }
    NODISCARD FORCEINLINE LInputAction const& GetAction(LUserInputTag Tag) const noexcept { auto* Result{algo::find_pointer(this->Actions, Tag, &LInputAction::Tag)}; check(Result) return *Result; }
    NODISCARD FORCEINLINE constexpr TArray<LInputAction> const& GetRegisteredActions() const noexcept { return this->Actions; }
    NODISCARD FORCEINLINE constexpr TArray<LInputAction>& GetMutableRegisteredActions() noexcept { return this->Actions; }

private:

    ENGINE_API void OnActionMapped(LInputAction& Action) noexcept;

    LUserInputTag Tag;
    LString DisplayName;
    TArray<LInputAction> Actions;
};

} /* ~Namespace Jafg */
