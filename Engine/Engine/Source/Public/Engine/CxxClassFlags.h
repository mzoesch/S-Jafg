// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

typedef u8 LCxxClassFlags;

namespace ECxxClassFlags
{

enum Type : LCxxClassFlags
{
    //# No flags are set. This is the default value.
    None            = 0 << 0,

    //# The class is abstract and can therefore not be instantiated.
    Abstract        = 1 << 0,

    //# Fields with the CLASS_FIELD macro are serialized both ways - only for the default package.
    Config          = 1 << 1,

    //# The class is a singleton and can only be accessed through the #DetDefault and #GetMutableDefault functions.
    Singleton       = Abstract,
};

} /* ~Namespace ECxxClassFlags */
ENUM_CLASS_FLAGS(ECxxClassFlags::Type)

namespace Jafg
{

template<typename... TFlags>
inline constexpr ECxxClassFlags::Type CombineCxxClassFlags(TFlags&&... Flags)
{
    static_assert((std::is_same_v<TFlags, ECxxClassFlags::Type> &&...));

    if constexpr (sizeof...(Flags) > 0)
    {
        return static_cast<ECxxClassFlags::Type>(ECxxClassFlags::None | (Flags |...));
    }

    return ECxxClassFlags::None;
}

} /* ~Namespace Jafg */
