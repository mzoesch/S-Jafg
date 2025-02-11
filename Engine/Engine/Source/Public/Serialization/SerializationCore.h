// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

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
FORCEINLINE void Deserialize<uint8>(uint8* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<uint8>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<uint16>(uint16* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<uint16>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<uint32>(uint32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<uint32>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<uint64>(uint64* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<uint64>(std::stoull(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<int8>(int8* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<int8>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<int16>(int16* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<int16>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<int32>(int32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<int32>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<int64>(int64* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = static_cast<int64>(std::stoll(InValue.ToPtr()));
}

template <>
FORCEINLINE void Deserialize<bool>(bool* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = InValue == "1";
}

} /* ~Namespace Jafg */
