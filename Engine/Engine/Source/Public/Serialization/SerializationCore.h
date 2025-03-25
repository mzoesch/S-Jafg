// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

template <typename TField>
void Deserialize(TField* Destination, const LString& InValue) UNSUPPORTED_TEMPLATED_SPECIALIZATION(TField)

template <typename TField>
LString Serialize(const TField& InValue) { return LString::SprintF("{}", InValue); }

template <>
FORCEINLINE void Deserialize<float>(float* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = std::stof(InValue.ToPtr());
}

template <>
FORCEINLINE void Deserialize<double>(double* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = std::stod(InValue.ToPtr());
}

template <>
FORCEINLINE void Deserialize<u8>(u8* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<u8>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<u16>(u16* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<u16>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<u32>(u32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<u32>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<u64>(u64* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<u64>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<i8>(i8* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<i8>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<i16>(i16* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<i16>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<i32>(i32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<i32>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<i64>(i64* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<i64>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<bool>(bool* Destination, const LString& InValue)
{
    checkSlow( Destination )
    if (InValue == "false" || InValue == "0")
    {
        *Destination = false;
        return;
    }

    if (InValue == "true" || InValue == "1")
    {
        *Destination = true;
        return;
    }

    panicMsgf( "Invalid boolean value [{}].", InValue.ToPtr() )

    return;
}

} /* ~Namespace Jafg */
