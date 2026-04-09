// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LProgramArgument final
{
    LString Identifier;
    std::variant<std::monostate, LString, TArray<LString>> Variant;

    inline constexpr bool IsStoreTrue() const noexcept { return std::holds_alternative<std::monostate>(this->Variant); }
    inline constexpr bool IsValue() const noexcept { return std::holds_alternative<LString>(this->Variant); }
    inline constexpr LString const& GetValue() const noexcept { check(this->IsValue()) return std::get<LString>(this->Variant); }
    inline constexpr bool IsList() const noexcept { return std::holds_alternative<TArray<LString>>(this->Variant); }
    inline constexpr TArray<LString> const& GetList() const noexcept { check(this->IsList()) return std::get<TArray<LString>>(this->Variant); }
};

} /* ~Namespace Jafg */

