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
struct ENGINE_API LInputAction final
{
    LInputAction() = default;

    LInputAction
    (
        const LName InName,
        const EInputActionCategory::Type InCategory
    ) :  Name(InName), DisplayName(Strings::AddSpacesToCamelCase(InName.ToString())), Category(InCategory) {  }

    LInputAction
    (
        LName InName,
        const LString& InDisplayName,
        const EInputActionCategory::Type InCategory
    ) : Name(std::move(InName)), DisplayName(InDisplayName), Category(InCategory) { }

    LInputAction
    (
        const LString& InDisplayName,
        const EInputActionCategory::Type InCategory
    ) : Name(MAKE_DYNAMIC_NAME(InDisplayName)), DisplayName(InDisplayName), Category(InCategory) {  }

    DEFAULT_REALLOC_OF_ANY_FORM(LInputAction)

    ~LInputAction() = default;

    FORCEINLINE bool operator==(const LInputAction& InOther) const { return this->Name == InOther.Name; }
    FORCEINLINE bool operator!=(const LInputAction& InOther) const { return this->Name != InOther.Name; }
    FORCEINLINE bool operator==(const LName& InOther) const { return this->Name == InOther; }
    FORCEINLINE bool operator!=(const LName& InOther) const { return this->Name != InOther; }

    FORCEINLINE const LName& GetName() const { return this->Name; }
    FORCEINLINE const LString& GetDisplayName() const { return this->DisplayName; }
    FORCEINLINE EInputActionCategory::Type GetCategory() const { return this->Category; }

private:

    LName Name;
    LString DisplayName;
    EInputActionCategory::Type Category { EInputActionCategory::None };
};

FORCEINLINE bool operator==(const LInputAction* InA, const LName& InB) { return InA && InA->GetName() == InB; }
FORCEINLINE bool operator!=(const LInputAction* InA, const LName& InB) { return !(InA == InB); }

} /* ~Namespace Jafg */
