// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

struct LInputListItem
{
    constexpr LInputListItem() noexcept = default;
    constexpr LInputListItem(LString Identifier) noexcept: Identifier{std::move(Identifier)} {}
    constexpr LInputListItem(LString Identifier, LString DisplayName, bool bSelectable = true) noexcept
        : Identifier{std::move(Identifier)}
    , DisplayName{std::move(DisplayName)}
    , bSelectable{bSelectable}
    {}

    LString Identifier;
    LString DisplayName;
    bool bSelectable{ true };
};

} /* ~Namespace Jafg */
