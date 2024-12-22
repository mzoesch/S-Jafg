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

    LKey  Key   = EKeys::Unresolved;
    float Value = 0.0f;

    FORCEINLINE bool operator <(const LRawInput& Other) const   { return this->Key < Other.Key;  }
    FORCEINLINE bool operator <(const LKey& Other) const        { return this->Key < Other;      }
    FORCEINLINE bool operator >(const LRawInput& Other) const   { return this->Key > Other.Key;  }
    FORCEINLINE bool operator >(const LKey& Other) const        { return this->Key > Other;      }
    FORCEINLINE bool operator==(const LRawInput& Other) const   { return this->Key == Other.Key; }
    FORCEINLINE bool operator==(const LKey& Other) const        { return this->Key == Other;     }
    FORCEINLINE bool operator!=(const LRawInput& Other) const   { return this->Key != Other.Key; }
    FORCEINLINE bool operator!=(const LKey& Other) const        { return this->Key != Other;     }

    FORCEINLINE void Reset()
    {
        this->Key   = EKeys::Unresolved;
        this->Value = 0.0f;
    }

    FORCEINLINE LSimpleString ToString() const
    {
        return LSimpleString::SprintF("{{{}: {:.2f}}}", LexToString(Key), Value);
    }
};

} /* ~Namespace Jafg */
