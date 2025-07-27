// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{


/*----------------------------------------------------------------------------
    Forwards.
----------------------------------------------------------------------------*/

template <typename TField>
void Deserialize(TField* Destination, const LString& InValue) UNSUPPORTED_TEMPLATED_SPECIALIZATION(TField)

template <typename TField>
LString Serialize(const TField& InValue) { return LString::SprintF("{}", InValue); }


/*----------------------------------------------------------------------------
    Deserialize.
----------------------------------------------------------------------------*/

template <>
FORCEINLINE void Deserialize<f32>(f32* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = std::stof(InValue.ToPtr());
}

template <>
FORCEINLINE void Deserialize<f64>(f64* Destination, const LString& InValue)
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

template <>
FORCEINLINE void Deserialize<LString>(LString* Destination, const LString& InValue)
{
    checkSlow( Destination )
    *Destination = InValue;
}

template <>
FORCEINLINE void Deserialize<LColor>(LColor* Destination, const LString& InValue)
{
    checkSlow( Destination )

    jassert( InValue.StartsWith("0x") )
    jassert( InValue.GetRuneCount() == 10 )

    *Destination = LColor::Transparent;

    auto GetValue
    {
        [](const LString& LambdaValue, const i32 LambdaIndex) -> u8
        {
            jassert
            (
                   (LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
                || (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F')
            )

            return
                (LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
                    ? (LambdaValue[LambdaIndex] - '0')
                    : (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F')
                        ? (LambdaValue[LambdaIndex] - 'A' + 10)
                        : 0;
        }
    };

    /*
     * This can be written simpler, but then we need two versions...
     * One for little endian and one for big endian platforms.
     */

    Destination->R = GetValue(InValue, 2);
    Destination->R <<= 4;
    Destination->R |= GetValue(InValue, 3);

    Destination->G = GetValue(InValue, 4);
    Destination->G <<= 4;
    Destination->G |= GetValue(InValue, 5);

    Destination->B = GetValue(InValue, 6);
    Destination->B <<= 4;
    Destination->B |= GetValue(InValue, 7);

    Destination->A = GetValue(InValue, 8);
    Destination->A <<= 4;
    Destination->A |= GetValue(InValue, 9);

    return;
}

template <typename TField>
FORCEINLINE void Deserialize(TArray<TField>* Destination, const LString& InValue)
{
    checkSlow( Destination )

    TArray<TField> Intermediate;

    const auto AddToIntermediate = [&Intermediate](const LString& Lambda) -> void
    {
        TField Temp;
        Deserialize<TField>(&Temp, Lambda);
        Intermediate.Add(Temp);

        return;
    };

    LString Temp;
    const i32 Count = InValue.GetRuneCount();
    for (i32 i = 0; i < Count; ++i)
    {
        if (i == 0 || i == Count - 1)
        {
            check( InValue[i] == '[' || InValue[i] == ']' )
            continue;
        }

        if (InValue[i] == ',')
        {
            if (Temp.IsEmpty())
            {
                continue;
            }

            AddToIntermediate(Temp);
            Temp.Empty();
            continue;
        }

        Temp += InValue[i];

        continue;
    }

    if (Temp.IsEmpty() == false)
    {
        AddToIntermediate(Temp);
        Temp.Empty();
    }

    if (Destination->IsDataUnequal(Intermediate))
    {
        *Destination = std::move(Intermediate);
    }

    return;
}


/*----------------------------------------------------------------------------
    Serialize.
----------------------------------------------------------------------------*/

template <typename TField>
FORCEINLINE LString Serialize(const TArray<TField>& InValue)
{
    LString Result = "[";

    for (typename TArray<TField>::SizeType Index { 0 }; Index < InValue.GetSize(); ++Index)
    {
        Result += Serialize(InValue[Index]);
        if (Index < InValue.GetSize() - 1)
        {
            Result += ',';
        }

        continue;
    }

    Result += "]";

    return Result;
}

template <>
FORCEINLINE LString Serialize<LColor>(const LColor& InValue)
{
    return LString::SprintF("0x{:02X}{:02X}{:02X}{:02X}", InValue.R, InValue.G, InValue.B, InValue.A);
}

} /* ~Namespace Jafg */
