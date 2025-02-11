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
FORCEINLINE void Deserialize<int32>(int32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = std::stoi(InValue.ToPtr());
}

template <>
FORCEINLINE void Deserialize<bool>(bool* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = InValue == "1";
}

} /* ~Namespace Jafg */
