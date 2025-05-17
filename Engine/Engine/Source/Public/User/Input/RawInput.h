// Copyright mzoesch. All rights reserved.

#pragma once

#include "InputTypes.h"

namespace Jafg
{

struct LRawInput
{
    FORCEINLINE constexpr explicit LRawInput() noexcept : Key(EKeys::Unresolved), Value(0.0f) { }
    FORCEINLINE constexpr explicit LRawInput(const LKey Key) noexcept : Key(Key), Value(1.0f) { }
    FORCEINLINE constexpr explicit LRawInput(const LKey Key, const f32 Value) noexcept : Key(Key), Value(Value){ }

    LKey Key;
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
        this->Key   = EKeys::Unresolved;
        this->Value = 0.0f;
    }

    FORCEINLINE LString ToString() const
    {
        return LString::SprintF("{{{}: {:.2f}}}", LexToString(Key), Value);
    }
};

} /* ~Namespace Jafg */
