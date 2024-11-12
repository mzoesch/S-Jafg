// Copyright mzoesch. All rights reserved.

#pragma once

#include "InputTypes.h"

namespace Jafg
{

struct LRawInput
{
    explicit LRawInput() = default;
    FORCEINLINE explicit LRawInput(const LKey Key) : Key(Key), Value(1.0f) { }
    FORCEINLINE explicit LRawInput(const LKey Key, const float Value) : Key(Key), Value(Value)
    {
    }

    LKey  Key;
    float Value;

    FORCEINLINE bool operator <(const LRawInput& Other) const   { return this->Key < Other.Key;  }
    FORCEINLINE bool operator <(const LKey& Other) const        { return this->Key < Other;      }
    FORCEINLINE bool operator >(const LRawInput& Other) const   { return this->Key > Other.Key;  }
    FORCEINLINE bool operator >(const LKey& Other) const        { return this->Key > Other;      }
    FORCEINLINE bool operator==(const LRawInput& Other) const   { return this->Key == Other.Key; }
    FORCEINLINE bool operator==(const LKey& Other) const        { return this->Key == Other;     }
    FORCEINLINE bool operator!=(const LRawInput& Other) const   { return this->Key != Other.Key; }
    FORCEINLINE bool operator!=(const LKey& Other) const        { return this->Key != Other;     }
};

} /* ~Namespace Jafg */
