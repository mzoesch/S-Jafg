// Copyright mzoesch. All rights reserved.
#pragma once

#include "User/Preferences/PreferenceTypes.h"

namespace Jafg
{

namespace EPolygonMode
{

enum Type : u8
{
    Fill      = 0,
    Wireframe = 1,
    Max       = 1,
    Num       = 2,
};

} /* ~Namespace EPolygonMode */
template<> struct TIsPreferenceTypeAllowed<EPolygonMode::Type> : std::true_type { };

namespace EApplicationScale
{

enum Type : u8
{
    Auto,
    Single,
    Double,
    Triple
};

} /* ~Namespace EApplicationScale */
template<> struct TIsPreferenceTypeAllowed<EApplicationScale::Type> : std::true_type { };

FORCEINLINE f32 LexToFloat(EApplicationScale::Type Scale) noexcept
{
    switch (Scale)
    {
    case EApplicationScale::Single: return 1.0f;
    case EApplicationScale::Double: return 2.0f;
    case EApplicationScale::Triple: return 3.0f;
    default:                        checkNoEntry()
    }

    return 1.0f;
}

} /* ~Namespace Jafg */

ENUM_CLASS_SERIALIZATION_FUNCTIONS(Jafg::EPolygonMode::Type)
ENUM_CLASS_SERIALIZATION_FUNCTIONS(Jafg::EApplicationScale::Type)
