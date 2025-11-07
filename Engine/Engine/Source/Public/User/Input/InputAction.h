// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/Name.h"
#include "User/Input/InputActionCategory.h"
#include "Containers/MyStringUtility.h"

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
    LInputAction() noexcept = default;

    LInputAction
    (
        const LName InName,
        const EInputActionCategory::Type InCategory
    ) noexcept
        : Name(InName), DisplayName(Strings::AddSpacesToCamelCase(InName.ToString())), Category(InCategory) {  }

    LInputAction
    (
        LName InName,
        LString InDisplayName,
        const EInputActionCategory::Type InCategory
    ) noexcept
        : Name(std::move(InName)), DisplayName(std::move(InDisplayName)), Category(InCategory) { }

    LInputAction
    (
        LStringView InDisplayName,
        const EInputActionCategory::Type InCategory
    ) noexcept
        : Name(MAKE_NAME(InDisplayName)), DisplayName(InDisplayName), Category(InCategory) {  }

    DEFAULT_REALLOC_OF_ANY_FORM(LInputAction)

    ~LInputAction() noexcept = default;

    FORCEINLINE bool operator==(LInputAction const& Other) const noexcept { return this->Name == Other.Name; }

    FORCEINLINE LName const& GetName() const noexcept { return this->Name; }
    FORCEINLINE LString const& GetDisplayName() const noexcept { return this->DisplayName; }
    FORCEINLINE EInputActionCategory::Type GetCategory() const noexcept { return this->Category; }

private:

    LName Name;
    LString DisplayName;
    EInputActionCategory::Type Category{ EInputActionCategory::None };
};

} /* ~Namespace Jafg */
