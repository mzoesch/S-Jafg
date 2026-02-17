// Copyright mzoesch. All rights reserved.

#pragma once

namespace Serde
{

//# The base struct for all serializers.
template<typename T, typename TArchive>
struct TSerializer final
{
    TSerializer() = delete;
};

//# The base struct for all deserializers.
template<typename T, typename TArchive>
struct TDeserializer final
{
    TDeserializer() = delete;
};

//# The result of a deserialization.
struct LDeserializationResult
{
    std::errc Errc;
    TOptional<LString> Error;
};

//# Whether T is serializable.
template<typename T, typename TArchive>
concept CSerializable = std::is_default_constructible_v<TSerializer<T, TArchive>>
    && requires(TSerializer<T, TArchive> const& F, TArchive& Ar, T const& t){{F(Ar, t)}->std::same_as<void>;};

//# Whether T is deserializable.
template<typename T, typename TArchive>
concept CDeserializable = std::is_default_constructible_v<TDeserializer<T, TArchive>>
    && requires(TDeserializer<T, TArchive> const& F, TArchive const& Ar, T& t){{F(Ar, t)}->std::same_as<LDeserializationResult>;};

//# Whether T can be serialized and deserialized.
// template<typename T, typename TArchive>
// concept CTwoWaySerializable = CSerializable<T, TArchive> && CDeserializable<T, TArchive>;

template<typename TArchive>
inline constexpr bool IsValidIArchiveStreamType_v{
       requires(TArchive Ar){{Ar.Stream}->std::same_as<LString&>;}
    || requires(TArchive Ar){{Ar.Stream}->std::same_as<LStringView&>;}
    };

//# Struct to signal an archive to copy prefs.
struct LCopyPrefs final{};

template<typename TArchive>
inline constexpr bool IsTextArchive_v{ std::is_same_v<typename TArchive::value_type, LJafgChar> };
template<typename TArchive>
inline constexpr bool IsBinaryArchive_v{ std::is_same_v<typename TArchive::value_type, u8> };

template<typename TArchive>
inline constexpr bool IsTextOArchive_v{ IsTextArchive_v<TArchive> && requires(TArchive Ar){{Ar.Stream}->std::same_as<std::stringstream&>;} };
template<typename TArchive>
inline constexpr bool IsTextIArchive_v{ IsTextArchive_v<TArchive> && IsValidIArchiveStreamType_v<TArchive> };

template<typename TArchive, typename... TArgs>
inline constexpr bool IsArchivePrefsCopyable_v{ std::is_constructible_v<TArchive, LCopyPrefs, TArchive const&, TArgs&&...> };

enum struct EBehavior
{
    Panic,
    Log,
    Ignore,
};

//# Lightweight default archive for output streams. Panics if anything goes wrong.
struct LOStringArchive final
{
    typedef LJafgChar value_type;

    constexpr LOStringArchive() noexcept = default;
    constexpr LOStringArchive(LCopyPrefs, LOStringArchive const&) noexcept {}
    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LOStringArchive)
    constexpr ~LOStringArchive() noexcept = default;

    template<typename T>
    inline decltype(auto) operator<<(this auto&& Self, T const& t) noexcept
        requires CSerializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        TSerializer<T, LOStringArchive>{}(Self, t);
        return std::forward<decltype(Self)>(Self);
    }

    std::stringstream Stream;
};

//# Lightweight default archive for input streams. Panics if anything goes wrong.
template<typename TStream, EBehavior Behavior> requires std::is_same_v<LString, TStream> || std::is_same_v<LStringView, TStream>
struct LIStringArchive final
{
    typedef LJafgChar value_type;

    constexpr LIStringArchive(TStream&& InStream) noexcept
        requires std::is_same_v<TStream, LString>
        : Stream(std::move(InStream)) {}
    constexpr LIStringArchive(TStream InStream) noexcept
        requires std::is_same_v<TStream, LStringView>
        : Stream(InStream) {}
    constexpr LIStringArchive(LCopyPrefs, LIStringArchive const&, TStream&& InStream) noexcept
        requires std::is_same_v<TStream, LString>
        : Stream(std::move(InStream)) {}
    constexpr LIStringArchive(LCopyPrefs, LIStringArchive const&, TStream InStream) noexcept
        requires std::is_same_v<TStream, LStringView>
        : Stream(InStream) {}

    DEFAULT_CONSTEXPR_REALLOC_OF_ANY_FORM(LIStringArchive)
    constexpr ~LIStringArchive() noexcept = default;

    template<typename T> requires (Behavior == EBehavior::Panic)
    inline decltype(auto) operator>>(this auto&& Self, T* t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        check(t)
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, *t)}; R.Errc != decltype(R.Errc){})
        {
            if (R.Error.has_value())
            {
                LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", Jafg::GetTypeName<T>(), *R.Error)
            }
            panicMsgf("Serialization from string to [{}] failed with [{}]", Jafg::GetTypeName<T>(), std::to_underlying(R.Errc))
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T> requires (Behavior != EBehavior::Panic)
    inline bool operator>>(this auto&& Self, T* t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        check(t)
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, *t)}; R.Errc != decltype(R.Errc){})
        {
            if constexpr (Behavior == EBehavior::Log || Behavior == EBehavior::Panic)
            {
                if (R.Error.has_value())
                {
                    LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", Jafg::GetTypeName<T>(), *R.Error)
                }
            }
           return false;
        }
        return true;
    }

    TStream Stream;
};

template<typename T, typename TArchive> requires CSerializable<T, TArchive>
    && IsTextOArchive_v<TArchive>
    && IsArchivePrefsCopyable_v<TArchive>
struct TSerializer<TArray<T>, TArchive>
{
    void operator()(TArchive& Ar, TArray<T> const& Field) const noexcept
    {
        auto& Ss{Ar.Stream};
        Ss << '[';

        bool bFirst{true};
        for (T const& Elem : Field)
        {
            if (!bFirst)
            {
                Ss << ',';
            }
            bFirst = false;

            TArchive IntermediateAr{LCopyPrefs{}, Ar};
            TSerializer<T, TArchive>{}(IntermediateAr, Elem);
            for (LString Intermediate{IntermediateAr.Stream.str()}; auto const& C : Intermediate)
            {
                if (C == '[' || C == ']' || C == ',' || C == '\\')
                {
                    Ss << '\\';
                }
                Ss << C;
            }
        }

        Ss << ']';
        return;
    }
};

template<typename T, typename TArchive> requires CDeserializable<T, TArchive>
    && IsTextIArchive_v<TArchive>
    && IsArchivePrefsCopyable_v<TArchive, LString&&>
struct TDeserializer<TArray<T>, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, TArray<T>& Field) const noexcept
    {
        auto& Value{Ar.Stream};
        TArray<T> Out;

        if (Value.starts_with('[') == false)
        {
            return {.Errc=std::errc::invalid_argument, .Error=Jafg::SprintF("Expected '[' at the start of: \n{}", Value)};
        }
        if (Value.ends_with(']') == false)
        {
            return {.Errc=std::errc::invalid_argument, .Error=Jafg::SprintF("Expected ']' at the end of: \n{}", Value)};
        }

        const LStringView View{Value.begin() + 1, Value.end() - 1};
        if (View.empty())
        {
            Field = std::move(Out);
            return {};
        }

        LString Element;
        bool bEscaped{};
        bool bTrailingComma{};
        for (const auto Char : View)
        {
            if (bEscaped)
            {
                Element += Char;
                bEscaped = false;
                continue;
            }

            bTrailingComma = false;

            if (Char == '\\')
            {
                bEscaped = true;
                continue;
            }

            if (Char == ',')
            {
                bTrailingComma = true;

                Out.emplace_back();
                TArchive IntermediateAr{LCopyPrefs{}, Ar, std::move(Element)};
                if (auto R{TDeserializer<T, TArchive>{}(IntermediateAr, Out.back())}; R.Errc != decltype(R.Errc){})
                {
                    return {
                        .Errc = R.Errc,
                        .Error = Jafg::SprintF("Subargument failed with [{}] at [{}]."
                            , R.Error.has_value() ? R.Error.value() : "<unknown error>"
                            , Element
                            )
                        };
                }

                Element.clear();
                continue;
            }

            Element += Char;
            continue;
        }

        if (bTrailingComma)
        {
            check(Element.empty())
        }
        else
        {
            Out.emplace_back();
            TArchive IntermediateAr{LCopyPrefs{}, Ar, std::move(Element)};
            if (auto R{TDeserializer<T, TArchive>{}(IntermediateAr, Out.back())}; R.Errc != decltype(R.Errc){})
            {
                return {
                    .Errc = R.Errc,
                    .Error = Jafg::SprintF("Subargument failed with [{}] at [{}]."
                        , R.Error.has_value() ? R.Error.value() : "<unknown error>"
                        , Element
                        )
                    };
            }
        }

        Field = std::move(Out);
        return {};
    }
};

template<typename T, typename TArchive> requires std::is_same_v<T, LString> || std::is_same_v<T, LStringView>
    && IsTextOArchive_v<TArchive>
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        Ar.Stream << Field;
    }
};
template<typename TArchive> requires IsTextIArchive_v<TArchive>
struct TDeserializer<LString, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LString& Field) const noexcept
    {
        Field.assign(Ar.Stream);
        return {};
    }
};
template<typename TArchive> requires IsTextIArchive_v<TArchive>
struct TDeserializer<LStringView, TArchive>
{
    //# Cannot store intermediate text in a non onwing object.
    constexpr TDeserializer() noexcept = delete;
};

template<typename T, typename TArchive> requires std::is_integral_v<T> || std::is_floating_point_v<T>
    && IsTextOArchive_v<TArchive>
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        Ar.Stream << Field;
    }
};
template<typename T, typename TArchive> requires std::is_integral_v<T> || std::is_floating_point_v<T>
    && IsTextIArchive_v<TArchive>
struct TDeserializer<T, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, T& Field) const noexcept
    {
        auto R{std::from_chars(algo::data(Ar.Stream), algo::data(Ar.Stream) + algo::size(Ar.Stream), Field)};
        return {
            .Errc = R.ec,
            .Error = R.ptr ? LString{R.ptr} : TOptional<LString>{},
            };
    }
};

template<typename TArchive> requires IsTextOArchive_v<TArchive>
struct TSerializer<bool, TArchive>
{
    void operator()(TArchive& Ar, bool const& Field) const noexcept
    {
        if (Field)
        {
            Ar.Stream << "1";
        }
        else
        {
            Ar.Stream << "0";
        }
    }
};
template<typename TArchive> requires IsTextIArchive_v<TArchive>
struct TDeserializer<bool, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, bool& Field) const noexcept
    {
        auto& Value{Ar.Stream};
        if (Value == "false" || Value == "0")
        {
            Field = false;
            return {};
        }
        if (Value == "true" || Value == "1")
        {
            Field = true;
            return {};
        }
        return {
            .Errc = std::errc::invalid_argument,
            .Error = Jafg::SprintF("Could not interpret [{}] as a boolean.", Value)
            };
    }
};


template<typename T, typename TArchive> requires std::is_enum_v<T>
    && IsTextOArchive_v<TArchive>
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        TSerializer<std::underlying_type_t<T>, TArchive>{}(Ar, std::to_underlying(Field));
    }
};
template<typename T, typename TArchive> requires std::is_enum_v<T>
    && IsTextIArchive_v<TArchive>
struct TDeserializer<T, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, T& Field) const noexcept
    {
        std::underlying_type_t<T> Temp{static_cast<std::underlying_type_t<T>>(Field)};
        auto R{TDeserializer<std::underlying_type_t<T>, TArchive>{}(Ar, Temp)};
        Field = static_cast<T>(Temp);
        return R;
    }
};

template<typename TArchive> requires IsTextOArchive_v<TArchive>
struct TSerializer<LColor, TArchive>
{
    void operator()(TArchive& Ar, LColor const& Field) const noexcept
    {
        Ar.Stream << Jafg::SprintF("0x{:02X}{:02X}{:02X}{:02X}", Field.R, Field.G, Field.B, Field.A);
    }
};
template<typename TArchive> requires IsTextIArchive_v<TArchive>
struct TDeserializer<LColor, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LColor& Field) const noexcept
    {
        auto& Value{Ar.Stream};

        if (Value.starts_with("0x") == false)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = Jafg::SprintF("Expected '0x' at the start of: {}", Value)
                };
        }

        if (Value.size() != 10)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = Jafg::SprintF("Expected a size of '10' but got '{}' from: {}", Value.size(), Value)
                };
        }

        for (auto Idx{2uz}; Idx < Value.size(); ++Idx)
        {
            if ((Value[Idx] >= '0' && Value[Idx] <= '9') || (Value[Idx] >= 'A' && Value[Idx] <= 'F'))
            {
                continue;
            }

            return{
                .Errc = std::errc::invalid_argument,
                .Error = Jafg::SprintF("Expected one of the values of [0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F] but got '{}' at index '{}' from: {}",
                    Value[Idx], Idx, Value
                    )
                };
        }

        Field = Colors::Transparent;

        auto GetValue{[](const LStringView& LambdaValue, const i32 LambdaIndex) -> u8
        {
            check((LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
               || (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F'))

            return
                (LambdaValue[LambdaIndex] >= '0' && LambdaValue[LambdaIndex] <= '9')
                    ? (LambdaValue[LambdaIndex] - '0')
                    : (LambdaValue[LambdaIndex] >= 'A' && LambdaValue[LambdaIndex] <= 'F')
                        ? (LambdaValue[LambdaIndex] - 'A' + 10)
                        : 0;
                }
        };

        Field.R = GetValue(Value, 2);
        Field.R <<= 4;
        Field.R |= GetValue(Value, 3);

        Field.G = GetValue(Value, 4);
        Field.G <<= 4;
        Field.G |= GetValue(Value, 5);

        Field.B = GetValue(Value, 6);
        Field.B <<= 4;
        Field.B |= GetValue(Value, 7);

        Field.A = GetValue(Value, 8);
        Field.A <<= 4;
        Field.A |= GetValue(Value, 9);

        return {};
    }
};

//# Quick conversion of the field T to a string with default formation.
template<typename T> requires CSerializable<T, LOStringArchive>
NODISCARD FORCEINLINE LString ToString(T const& Field) noexcept
{
    LOStringArchive Ar;
    Ar << Field;
    return Ar.Stream.str();
}

//# Quick conversion of the formation from T to T. If an error occurs the program will panic.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, EBehavior::Panic>>
FORCEINLINE void FromString(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, EBehavior::Panic> Ar{Value};
    Ar >> Field;
    return;
}

//# Quick conversion of the formation from T to T. If an error occurs it will return false and log a message.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, EBehavior::Log>>
FORCEINLINE bool FromStringLogged(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, EBehavior::Log> Ar{Value};
    return Ar >> Field;
}

//# Quick conversion of the formation from T to T. If an error occurs it will return false.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, EBehavior::Ignore>>
FORCEINLINE bool FromStringRelaxed(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, EBehavior::Ignore> Ar{Value};
    return Ar >> Field;
}

} /* ~Namespace Serde */
