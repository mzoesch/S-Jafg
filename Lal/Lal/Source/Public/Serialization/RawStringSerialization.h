// Copyright mzoesch. All rights reserved.

#pragma once

namespace Serialization
{

//# Default
template<typename TField> NODISCARD FORCEINLINE constexpr LString ToString(TField const& Field) noexcept = delete;
template<typename TField> FORCEINLINE constexpr void FromString(TField* Field, LString const& Value) noexcept = delete;

//#
//# Safe conversion from string.
//# @return True if conversion was successful, false otherwise.
//# @note If false is returned, the value of #Field is undefined.
//#
template<typename TField> FORCEINLINE constexpr bool FromStringSafe(TField* Field, LString const& Value, LString* OutError = nullptr) noexcept = delete;

template<> NODISCARD FORCEINLINE constexpr LString ToString<LString>(LString const& Field) noexcept { return Field; }
template<> NODISCARD FORCEINLINE constexpr LString ToString<LStringView>(LStringView const& Field) noexcept { return LString{Field}; }
template<> FORCEINLINE constexpr void FromString<LString>(LString* Field, LString const& Value) noexcept { check( Field ) *Field = Value; }
template<> FORCEINLINE constexpr void FromString<LStringView>(LStringView* Field, LString const& Value) noexcept = delete;

namespace Private
{

template<typename T> requires std::is_integral_v<T> || std::is_floating_point_v<T>
FORCEINLINE constexpr bool NumericFromStringSafe(T* Field, LString const& Value, LString* OutError = nullptr) noexcept
{
    auto [Ptr, Ec] = std::from_chars(Value.data(), Value.data() + Value.size(), *Field);
    if (Ec == std::errc() || Ptr != Value.data() + Value.size())
    {
        LString Error{ Lal::SprintF("Cannot convert [{}] to [{}]: [{}].", Value, Lal::GetTypeName<T>(), std::make_error_code(Ec).message()) };
        if (OutError)
        {
            *OutError = std::move(Error);
        }
        else
        {
            LOG_ERROR(LogSerialization, "{}", Error)
        }

        return false;
    }

    return true;
}

} /* ~Namespace Private */

///////////////////////////////////////////////////////////////////////////////
// Array
///////////////////////////////////////////////////////////////////////////////
template<typename TSubField> FORCEINLINE constexpr LString
ToString(TArray<TSubField> const& Field) noexcept
    requires requires(TSubField const& SubField){ ToString(SubField); }
{
    LString Result { "[" };

    bool bFirst { true };
    for (TSubField const& Elem : Field)
    {
        if (bFirst)
        {
            bFirst = false;
        }
        else
        {
            Result += ',';
        }

        for (LString Intermediate{ ToString<TSubField>(Elem) }; LChar const& C : Intermediate)
        {
            if (C == '[' || C == ']' || C == ',' || C == '\\')
            {
                Result += '\\';
            }

            Result += C;

            continue;
        }

        continue;
    }

    Result += ']';
    return Result;
}

template<typename TSubField> FORCEINLINE constexpr void
FromString(TArray<TSubField>* Field, LString const& Value) noexcept
    requires requires(TSubField* SubField, LString const& SubValue){ FromString(SubField, SubValue); }
{
    check( Field )

    TArray<TSubField> Out;

    jassert( Value.starts_with('[') && Value.ends_with(']') )

    const LStringView View{ Value.begin() + 1, Value.end() - 1 };
    if (View.empty())
    {
        *Field = std::move(Out);
        return;
    }

    LString Element;
    bool bEscaped{ false };
    for (const auto Char : View)
    {
        if (bEscaped)
        {
            Element += Char;
            bEscaped = false;
            continue;
        }

        if (Char == '\\')
        {
            bEscaped = true;
            continue;
        }

        if (Char == ',')
        {
            Out.emplace_back();
            FromString(&Out.back(), Element);

            Element.clear();
            continue;
        }

        Element += Char;

        continue;
    }

    *Field = std::move(Out);

    return;
}


///////////////////////////////////////////////////////////////////////////////
// Primitives.
///////////////////////////////////////////////////////////////////////////////
template<> NODISCARD FORCEINLINE constexpr LString ToString<bool>(bool const& Field) noexcept { if (Field) { return "1"; } return "0"; }
template<> FORCEINLINE constexpr void FromString<bool>(bool* Field, LString const& Value) noexcept
{
    check( Field )

    if (Value == "false" || Value == "0")
    {
        *Field = false;
        return;
    }

    if (Value == "true" || Value == "1")
    {
        *Field = true;
        return;
    }

    panicMsgf( "[{}] can not be interpreted as boolean.", Value )

    return;
}

static_assert(sizeof(i32) == sizeof(int), "stoi is not valid for i32.");
static_assert(sizeof(f32) == sizeof(float) , "stof is not valid for f32.");
static_assert(sizeof(f64) == sizeof(double), "stod is not valid for f64.");

template<> NODISCARD FORCEINLINE constexpr LString ToString<i8>(i8 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<i8>(i8* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<i8>(std::stoi(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<i8>(i8* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<i8>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<i16>(i16 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<i16>(i16* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<i16>(std::stoi(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<i16>(i16* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<i16>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<i32>(i32 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<i32>(i32* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<i32>(std::stoi(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<i32>(i32* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<i32>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<i64>(i64 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<i64>(i64* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<i64>(std::stoll(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<i64>(i64* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<i64>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<u8>(u8 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<u8>(u8* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<u8>(std::stoul(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<u8>(u8* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<u8>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<u16>(u16 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<u16>(u16* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<u16>(std::stoul(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<u16>(u16* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<u16>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<u32>(u32 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<u32>(u32* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<u32>(std::stoul(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<u32>(u32* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<u32>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<u64>(u64 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<u64>(u64* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<u64>(std::stoull(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<u64>(u64* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<u64>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<f32>(f32 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<f32>(f32* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<f32>(std::stof(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<f32>(f32* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<f32>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<f64>(f64 const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<f64>(f64* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<f64>(std::stod(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<f64>(f64* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<f64>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<LSize>(LSize const& Field) noexcept { return std::to_string(Field); }
template<> FORCEINLINE constexpr void FromString<LSize>(LSize* Field, LString const& Value) noexcept { check( Field ) *Field = static_cast<LSize>(std::stoull(Value) ); }
template<> FORCEINLINE constexpr bool FromStringSafe<LSize>(LSize* Field, LString const& Value, LString* OutError /* = nullptr */) noexcept { return Private::NumericFromStringSafe<LSize>(Field, Value, OutError); }

template<> NODISCARD FORCEINLINE constexpr LString ToString<Lal::LColor>(Lal::LColor const& Field) noexcept
{
    return Lal::SprintF("0x{:02X}{:02X}{:02X}{:02X}", Field.R, Field.G, Field.B, Field.A);
}
template<> FORCEINLINE constexpr void FromString<Lal::LColor>(Lal::LColor* Field, LString const& Value) noexcept
{
    check( Field )

    jassert( Value.starts_with("0x") )
    jassert( Value.size() == 10 )

    *Field = Lal::LColor::Transparent;

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

    Field->R = GetValue(Value, 2);
    Field->R <<= 4;
    Field->R |= GetValue(Value, 3);

    Field->G = GetValue(Value, 4);
    Field->G <<= 4;
    Field->G |= GetValue(Value, 5);

    Field->B = GetValue(Value, 6);
    Field->B <<= 4;
    Field->B |= GetValue(Value, 7);

    Field->A = GetValue(Value, 8);
    Field->A <<= 4;
    Field->A |= GetValue(Value, 9);

    return;
}

} /* ~Namespace Serialization */

#define ENUM_CLASS_SERIALIZATION_FUNCTIONS(EnumType)                                                              \
    template<> FORCEINLINE LString Serialization::ToString<EnumType>(EnumType const& Field) noexcept              \
    {                                                                                                             \
        static_assert(std::is_enum_v<EnumType>);                                                                  \
        return ToString(static_cast<std::underlying_type_t<EnumType>>(Field));                                    \
    }                                                                                                             \
    template<> FORCEINLINE void Serialization::FromString<EnumType>(EnumType* Dst, LString const& Value) noexcept \
    {                                                                                                             \
        static_assert(std::is_enum_v<EnumType>);                                                                  \
        check( Dst )                                                                                              \
        FromString(reinterpret_cast<std::underlying_type_t<EnumType>*>(Dst), Value);                              \
        return;                                                                                                   \
    }
