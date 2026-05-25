// Copyright mzoesch. All rights reserved.

#pragma once

#include "User/Input/InputActionCategory.h"
#include "Misc/Tag.h"

namespace Jafg
{

//#
//# A form of an action that can be triggered by the user.
//# This is the logical representation of something the user wants to / can do (e.g., "Close frontend").
//# It is not tied to any specific physical key - but many physical keys can be bound to this action and can be
//# changed modularly based on delegates fired in the application / state of the program.
//#
struct LInputAction final
{
    constexpr LInputAction() noexcept = delete;
    LInputAction(LUserInputTag InTag, LString InDisplayName, EInputActionCategory::Type InCategory) noexcept
        : Tag(InTag), DisplayName(std::move(InDisplayName)), Category(InCategory) { check(this->Tag.IsSet()) }
    LInputAction(LUserInputTag InTag,EInputActionCategory::Type InCategory) noexcept
        : LInputAction{InTag, algo::add_spaces_to_camel_case(InTag.ToString()), InCategory} {}

    LInputAction(LStringView InDisplayName, EInputActionCategory::Type InCategory) noexcept
        : LInputAction{LUserInputTag::ToTag(InDisplayName), LString{InDisplayName}, InCategory} {}
    DEFAULT_REALLOC_OF_ANY_FORM(LInputAction)
    ~LInputAction() noexcept = default;

    FORCEINLINE bool operator==(LInputAction const& Other) const noexcept { return this->Tag == Other.Tag; }

    FORCEINLINE LUserInputTag const& GetTag() const noexcept { return this->Tag; }
    FORCEINLINE LString const& GetDisplayName() const noexcept { return this->DisplayName; }
    FORCEINLINE EInputActionCategory::Type GetCategory() const noexcept { return this->Category; }

private:

    LUserInputTag Tag;
    LString DisplayName;
    EInputActionCategory::Type Category{ EInputActionCategory::None };
};

} /* ~Namespace Jafg */
