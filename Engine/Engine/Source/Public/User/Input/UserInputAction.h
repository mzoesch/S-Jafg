// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "User/Input/InputActionModifiers.h"
#include "User/Input/InputActionCategory.h"

namespace Jafg
{

struct ENGINE_API LInputActionMappedKey final
{
    LInputActionMappedKey() = default;
    LInputActionMappedKey(const LKey InKey) : Key(InKey) { }
    PROHIBIT_REALLOC_OF_ANY_FROM(LInputActionMappedKey)
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
 */
struct ENGINE_API LUserInputAction final
{
    LUserInputAction() = default;
    explicit LUserInputAction(const EInputActionCategory::Type InCategory) : Category(InCategory) {}
    DEFAULT_REALLOC_OF_ANY_FROM(LUserInputAction)
    ~LUserInputAction() = default;

    EInputActionCategory::Type      Category = EInputActionCategory::None;
    TdhArray<LInputActionMappedKey> MappedKeys;
};

} /* ~Namespace Jafg */
