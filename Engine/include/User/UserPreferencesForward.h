// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

enum struct EPolygonMode : u8
{
    Fill,
    Wireframe,
};

enum struct EApplicationScale : u8
{
    Auto,
    Single,
    Double,
    Triple
};

inline constexpr f32 LexToFloat(EApplicationScale Scale) noexcept
{
    switch (Scale)
    {
    case EApplicationScale::Single: { return 1.0f; }
    case EApplicationScale::Double: { return 2.0f; }
    case EApplicationScale::Triple: { return 3.0f; }
    default:                        { checkNoEntry() }
    }
    return 1.0f;
}

inline constexpr f64 LexToDouble(EApplicationScale Scale) noexcept
{
    switch (Scale)
    {
    case EApplicationScale::Single: { return 1.0; }
    case EApplicationScale::Double: { return 2.0; }
    case EApplicationScale::Triple: { return 3.0; }
    default:                        { checkNoEntry() }
    }
    return 1.0;
}

} /* ~Namespace Jafg */

JAFG_PREF_OF(Jafg::EPolygonMode)
JAFG_PREF_OF(Jafg::EApplicationScale)
