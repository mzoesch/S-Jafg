// Copyright mzoesch. All rights reserved.

#pragma once

namespace Serde
{

template<typename T> struct TIsString : std::false_type {};
template<typename T> inline constexpr bool IsString_v{TIsString<T>::value};

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
    std::optional<LString> Error;
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
inline constexpr bool IsTextArchive_v{ std::is_same_v<typename TArchive::value_type, char> };
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
    typedef char value_type;

    constexpr LOStringArchive() noexcept = default;
    LOStringArchive(LCopyPrefs, LOStringArchive const&) noexcept {}
    PROHIBIT_COPY(LOStringArchive)
    DEFAULT_CONSTEXPR_MOVE(LOStringArchive)
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
    typedef char value_type;

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

    template<typename T> requires(Behavior == EBehavior::Panic)
    inline decltype(auto) operator>>(this auto&& Self, T& t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, t)}; R.Errc != decltype(R.Errc){})
        {
            if (R.Error.has_value())
            {
                LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", algo::type_name<T>(), *R.Error)
            }
            panicMsgf("Serialization from string to [{}] failed with [{}]", algo::type_name<T>(), std::to_underlying(R.Errc))
        }
        return std::forward<decltype(Self)>(Self);
    }

    template<typename T> requires(Behavior != EBehavior::Panic)
    inline bool operator>>(this auto&& Self, T& t) noexcept
        requires CDeserializable<T, std::remove_cvref_t<decltype(Self)>>
    {
        if (auto R{TDeserializer<T, LIStringArchive>{}(Self, t)}; R.Errc != decltype(R.Errc){})
        {
            if constexpr (Behavior == EBehavior::Log || Behavior == EBehavior::Panic)
            {
                if (R.Error.has_value())
                {
                    LOG_ERROR(LogSerialization, "Serialization from string to [{}] failed: ", algo::type_name<T>(), *R.Error)
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
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected '[' at the start of: \n{}", Value)};
        }
        if (Value.ends_with(']') == false)
        {
            return {.Errc=std::errc::invalid_argument, .Error=algo::sprintf("Expected ']' at the end of: \n{}", Value)};
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
                        .Error = algo::sprintf("Subargument failed with [{}] at [{}]."
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
                    .Error = algo::sprintf("Subargument failed with [{}] at [{}]."
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

template<typename T, typename TArchive> requires((std::is_same_v<T, LString> || std::is_same_v<T, LPath> || std::is_same_v<T, LStringView>)
    && IsTextOArchive_v<TArchive>)
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
struct TDeserializer<LPath, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, LPath& Field) const noexcept
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

template<typename T, typename TArchive> requires((std::is_integral_v<T> || std::is_floating_point_v<T>)
    && IsTextOArchive_v<TArchive>)
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        Ar.Stream << Field;
    }
};
template<typename T, typename TArchive> requires((std::is_integral_v<T> || std::is_floating_point_v<T>)
    && IsTextIArchive_v<TArchive>)
struct TDeserializer<T, TArchive>
{
    LDeserializationResult operator()(TArchive const& Ar, T& Field) const noexcept
    {
        auto R{std::from_chars(algo::data(Ar.Stream), algo::data(Ar.Stream) + algo::size(Ar.Stream), Field)};
        return {
            .Errc = R.ec,
            .Error = R.ptr ? LString{R.ptr} : std::optional<LString>{},
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
            .Error = algo::sprintf("Could not interpret [{}] as a boolean.", Value)
            };
    }
};


template<typename T, typename TArchive> requires std::is_enum_v<T>
    && IsTextOArchive_v<TArchive>
struct TSerializer<T, TArchive>
{
    void operator()(TArchive& Ar, T const& Field) const noexcept
    {
        // As this is a text o archive,
        // we have to avoid printing something like a char that is null, as this would end the string.
        if constexpr (std::is_signed_v<std::underlying_type_t<T>>)
        {
            TSerializer<i64, TArchive>{}(Ar, static_cast<i64>(std::to_underlying(Field)));
        }
        else
        {
            TSerializer<u64, TArchive>{}(Ar, static_cast<u64>(std::to_underlying(Field)));
        }
    }
};

//# TODO: Is is of course shit. Because we cannot verify the cast. But lets wait for c++26 to fix this...
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
        Ar.Stream << algo::sprintf("0x{:02X}{:02X}{:02X}{:02X}", Field.r, Field.g, Field.b, Field.a);
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
                .Error = algo::sprintf("Expected '0x' at the start of: {}", Value)
                };
        }

        if (Value.size() != 10)
        {
            return {
                .Errc = std::errc::invalid_argument,
                .Error = algo::sprintf("Expected a size of '10' but got '{}' from: {}", Value.size(), Value)
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
                .Error = algo::sprintf("Expected one of the values of [0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F] but got '{}' at index '{}' from: {}",
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

        Field.r = GetValue(Value, 2);
        Field.r <<= 4;
        Field.r |= GetValue(Value, 3);

        Field.g = GetValue(Value, 4);
        Field.g <<= 4;
        Field.g |= GetValue(Value, 5);

        Field.b = GetValue(Value, 6);
        Field.b <<= 4;
        Field.b |= GetValue(Value, 7);

        Field.a = GetValue(Value, 8);
        Field.a <<= 4;
        Field.a |= GetValue(Value, 9);

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
    Ar >> *Field;
    return;
}

//#
//# Quick conversion of the formation from T to T. If an error occurs it will return false and log a message
//# if logging for LogSerialization at error verbosity is enabled.
//#
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, EBehavior::Log>>
FORCEINLINE bool FromStringLogged(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, EBehavior::Log> Ar{Value};
    return Ar >> *Field;
}

//# Quick conversion of the formation from T to T. If an error occurs it will return false.
template<typename T> requires CDeserializable<T, LIStringArchive<LStringView, EBehavior::Ignore>>
FORCEINLINE bool FromStringRelaxed(T* Field, LStringView Value) noexcept
{
    LIStringArchive<LStringView, EBehavior::Ignore> Ar{Value};
    return Ar >> *Field;
}

template<> struct TIsString<LString> : std::true_type {};
template<> struct TIsString<Lu8String> : std::true_type {};
template<> struct TIsString<Lu16String> : std::true_type {};
template<> struct TIsString<Lu32String> : std::true_type {};
template<> struct TIsString<LStringView> : std::true_type {};
template<> struct TIsString<Lu8StringView> : std::true_type {};
template<> struct TIsString<Lu16StringView> : std::true_type {};
template<> struct TIsString<Lu32StringView> : std::true_type {};
template<> struct TIsString<char const*> : std::true_type {};
template<> struct TIsString<unsigned char const*> : std::true_type {};
template<> struct TIsString<signed char const*> : std::true_type {};
template<> struct TIsString<char8_t const*> : std::true_type {};
template<> struct TIsString<char16_t const*> : std::true_type {};
template<> struct TIsString<char32_t const*> : std::true_type {};

} /* ~Namespace Serde */

template<typename T> requires requires(T t) { {t.ToString()} -> std::convertible_to<LString>; }
struct std::formatter<T> : std::formatter<std::string>
{
    FORCEINLINE std::format_context::iterator format(T const& Value, std::format_context& Context) const
    {
        return std::formatter<std::string>::format(Value.ToString(), Context);
    }
};
