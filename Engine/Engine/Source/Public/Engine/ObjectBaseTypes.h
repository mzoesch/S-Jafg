// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

typedef uint8 LClassFlags;

/**
 * Class flags that can be used to describe a jafg object class.
 * @note This namespace resides outside the jafg namespace to avoid long typing when declaring new classes
 *       as the jafg namespace does not exist in the global namespace.
 */
namespace EClassFlags
{

enum Type : LClassFlags
{
    /** No flags are set. This is the default value. */
    None            = 0 << 0,

    /** The class is abstract and can therefore not be instantiated. */
    Abstract        = 1 << 0,

    /** The class is a singleton and can only be accessed through the #DetDefault and #GetMutableDefault functions. */
    Singleton       = Abstract,
};

FORCEINLINE Type operator|(const Type& Lhs, const Type& Rhs)
{
    return static_cast<Type>(static_cast<LClassFlags>(Lhs) | static_cast<LClassFlags>(Rhs));
}

FORCEINLINE Type& operator|=(Type& Lhs, const Type& Rhs)
{
    Lhs = Lhs | Rhs;
    return Lhs;
}

FORCEINLINE Type operator&(const Type& Lhs, const Type& Rhs)
{
    return static_cast<Type>(static_cast<LClassFlags>(Lhs) & static_cast<LClassFlags>(Rhs));
}

FORCEINLINE Type& operator&=(Type& Lhs, const Type& Rhs)
{
    Lhs = Lhs & Rhs;
    return Lhs;
}

FORCEINLINE Type operator~(const Type& Lhs)
{
    return static_cast<Type>(~static_cast<LClassFlags>(Lhs));
}

template <typename  ... FlagsTy>
constexpr EClassFlags::Type CombineFlags(FlagsTy ... Flags)
{
    static_assert(
        (std::is_same_v<FlagsTy, EClassFlags::Type> && ...),
        "All arguments must be of type EClassFlags::Type."
    );

    if constexpr (sizeof ... (Flags) > 0)
    {
        return EClassFlags::None | (static_cast<EClassFlags::Type>(Flags) | ...);
    }

    return EClassFlags::None;
}

} /* ~Namespace EClassFlags */
