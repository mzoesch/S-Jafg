// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LGenericArgument
{
    FORCEINLINE LGenericArgument() noexcept = default;
    FORCEINLINE LGenericArgument(LString InIdentifier) noexcept
        : Identifier{std::move(InIdentifier)} { return; }
    FORCEINLINE LGenericArgument(LString InIdentifier, LString InValue) noexcept
        : Identifier{std::move(InIdentifier)}, Value{std::move(InValue)} { return; }

    LString Identifier;
    std::optional<LString> Value;
    std::optional<TArray<LString>> Values;

    FORCEINLINE constexpr bool HasValue() const noexcept { return this->Value.has_value(); }
    FORCEINLINE constexpr bool HasValues() const noexcept { return this->Values.has_value(); }
};

} /* ~Namespace Jafg */
