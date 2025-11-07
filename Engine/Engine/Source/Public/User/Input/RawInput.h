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

    FORCEINLINE constexpr std::strong_ordering operator<=>(const LRawInput& Other) const noexcept { return this->Key <=> Other.Key; }

    FORCEINLINE constexpr void Reset() noexcept
    {
        this->Key = EKeys::Unresolved;
        this->bRepeated = false;
        this->Value = 0.0f;
    }

    FORCEINLINE LString ToString() const noexcept
    {
        return Lal::SprintF("{{{}: {:.2f}}}", LexToString(this->Key), this->Value);
    }
};

} /* ~Namespace Jafg */
