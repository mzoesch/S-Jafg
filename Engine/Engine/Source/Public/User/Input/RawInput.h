// Copyright mzoesch. All rights reserved.

#pragma once

#include "InputTypes.h"

namespace Jafg
{

struct LRawInput
{
    FORCEINLINE constexpr explicit LRawInput() noexcept : Key(EKeys::Unresolved), bRepeated(false), Value(0.0f) { }
    FORCEINLINE constexpr explicit LRawInput(const LKey Key) noexcept : Key(Key), bRepeated(false), Value(1.0f) { }
    FORCEINLINE constexpr explicit LRawInput(const LKey Key, const f32 Value) noexcept : Key(Key), bRepeated(false), Value(Value){ }

    LKey Key;
    bool bRepeated;
    f32  Value;

    FORCEINLINE constexpr bool operator <(const LRawInput& Other) const noexcept { return this->Key < Other.Key;  }
    FORCEINLINE constexpr bool operator <(const LKey& Other) const noexcept      { return this->Key < Other;      }
    FORCEINLINE constexpr bool operator >(const LRawInput& Other) const noexcept { return this->Key > Other.Key;  }
    FORCEINLINE constexpr bool operator >(const LKey& Other) const noexcept      { return this->Key > Other;      }
    FORCEINLINE constexpr bool operator==(const LRawInput& Other) const noexcept { return this->Key == Other.Key; }
    FORCEINLINE constexpr bool operator==(const LKey& Other) const noexcept      { return this->Key == Other;     }
    FORCEINLINE constexpr bool operator!=(const LRawInput& Other) const noexcept { return this->Key != Other.Key; }
    FORCEINLINE constexpr bool operator!=(const LKey& Other) const noexcept      { return this->Key != Other;     }

    FORCEINLINE void Reset() noexcept
    {
        this->Key = EKeys::Unresolved;
        this->bRepeated = false;
        this->Value = 0.0f;
    }

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{{{}: {:.2f}}}", LexToString(this->Key), this->Value);
    }
};

} /* ~Namespace Jafg */
