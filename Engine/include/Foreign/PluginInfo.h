// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

//#
//# Optional plugin info that plugins may use / derive for ease of use.
//#
struct LPluginInfo
{
    LPath Content;

    NODISCARD LPath operator/(auto&& Path) const noexcept
        requires requires(LPath const& P) { P / Path; }
    {
        check(!this->Content.empty())
        return this->Content / Path;
    }
};

} /* ~Namespace Jafg */
