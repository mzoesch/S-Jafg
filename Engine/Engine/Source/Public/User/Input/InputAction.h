// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputActionModifiers.h"
#include "User/Input/InputActionCategory.h"

namespace Jafg
{

/**
 * Represents a specific physical key on the keyboard or mouse that is mapped to an action.
 */
struct ENGINE_API LInputActionMappedKey final
{
    LInputActionMappedKey() = default;
    LInputActionMappedKey(const LKey InKey) : Key(InKey) { }
    PROHIBIT_REALLOC_OF_ANY_FORM(LInputActionMappedKey)
    ~LInputActionMappedKey()
    {
        for (const LInputActionMappedKeyModifier* Modifier : this->Modifiers)
        {
            delete Modifier;
        }
        this->Modifiers.Empty();
    }

    LKey Key = EKeys::AnyKey;
    TdhArray<LInputActionMappedKeyModifier*> Modifiers;
};

/**
 * A form of an action that can be triggered by the user.
 * This is the logical representation of something the user wants to / can do (e.g., "Close frontend").
 * It is not tied to any specific physical key - but many physical keys can be bound to this action and can be
 * changed modularly based on delegates fired in the application / state of the program.
 */
struct ENGINE_API LInputAction final
{
    LInputAction() = default;
    explicit LInputAction(const EInputActionCategory::Type InCategory) : Category(InCategory) { }
    DEFAULT_REALLOC_OF_ANY_FORM(LInputAction)
    ~LInputAction() = default;

    EInputActionCategory::Type      Category = EInputActionCategory::None;
    TdhArray<LInputActionMappedKey> MappedKeys;
};

} /* ~Namespace Jafg */
