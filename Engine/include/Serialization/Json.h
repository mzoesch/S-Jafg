// Copyright mzoesch. All rights reserved.

#pragma once

namespace Serde
{

template<typename T>
[[noreturn]]
void NlohmannSink(T const& Error) noexcept;

} /* ~Namespace Serde */

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
#endif /* JAFG_WITH_CLANG */
    #define JSON_NOEXCEPTION
    #define JSON_THROW_USER ::Serde::NlohmannSink
    #include "nlohmann/json.hpp"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

using json = nlohmann::json;

namespace Serde
{

template<typename T>
[[noreturn]]
inline void NlohmannSink(T const& Error) noexcept
{
    LOG_FATAL(LogSerialization, "{}", Error.what())
}

inline constexpr LStringView LexToString(json::value_t Value) noexcept
{
    switch (Value)
    {
    case json::value_t::null: return "null";
    case json::value_t::object: return "object";
    case json::value_t::array: return "array";
    case json::value_t::string: return "string";
    case json::value_t::boolean: return "boolean";
    case json::value_t::binary: return "binary";
    case json::value_t::discarded: return "discarded";
    case json::value_t::number_integer:
    case json::value_t::number_unsigned:
    case json::value_t::number_float:
    default: return "number";
    }
}

template<typename T, typename... TArgs>
[[noreturn]]
inline void JsonSink(json const& j, char const* Format, TArgs&&... Args)
{
    LOG_FATAL(LogSerialization, "[{}]: {}. Failed to load: \n{}"
        , algo::type_name<T>(), algo::sprintf(Format, std::forward<TArgs>(Args)...), j.dump(4))
}
template<typename T>
[[noreturn]]
inline void JsonKeySink(json const& j, LStringView Key)
{
    LOG_FATAL(LogSerialization, "[{}]: Missing key [{}]. Failed to load: \n{}"
        , algo::type_name<T>(), Key, j.dump(4))
}

template<typename T>
inline void JsonExpectType(json const& j, json::value_t Type) noexcept
{
    if (j.type() != Type)
    {
        JsonSink<T>(j, "Expected [{}] got [{}]", LexToString(Type), j.type_name());
    }

    return;
}
template<typename T>
inline void JsonExpectSize(json const& j, std::size_t Size) noexcept
{
    if (j.size() != Size)
    {
        JsonSink<T>(j, "Expected size [{}] got [{}]", Size, j.size());
    }

    return;
}

template<typename T>
inline void JsonExpectAtLeast(json const& j, std::size_t Size) noexcept
{
    if (j.size() < Size)
    {
        JsonSink<T>(j, "Expected [{} >= {}]", Size, j.size());
    }
}

template<typename T, typename... TArgs>
    requires ((std::same_as<std::decay_t<TArgs>, char const*> && ...) && sizeof...(TArgs) > 0)
inline void JsonExpectKeys(json const& j, TArgs&&... Args) noexcept
{
    JsonExpectType<T>(j, json::value_t::object);
    auto CheckKey = [&j](LStringView Key) noexcept
    {
        if (!j.contains(Key))
        {
            JsonKeySink<T>(j, Key);
        }
    };

    (CheckKey(Args), ...);

    return;
}

struct LJsonKey
{
    std::string_view Identifier;
    json::value_t Value;
};
template<typename T, typename... TArgs>
    requires ((std::same_as<std::decay_t<TArgs>, LJsonKey> && ...) && sizeof...(TArgs) > 0)
inline void JsonExpectKeyTypes(json const& j, TArgs&&... Args) noexcept
{
    JsonExpectType<T>(j, json::value_t::object);
    auto CheckKey = [&j](LJsonKey const& Key) noexcept
    {
        if (!j.contains(Key.Identifier))
        {
            JsonKeySink<T>(j, Key.Identifier);
        }
        JsonExpectType<T>(j.at(Key.Identifier), Key.Value);
    };

    (CheckKey(Args), ...);

    return;
}

} /* ~Namespace Serde */

namespace glm
{

template<length_t N, typename T, qualifier Q>
void to_json(json& j, vec<N, T,Q> const& v)
{
    if constexpr (N == 2)
    {
        j = json::array({v.x, v.y});
    }
    else if constexpr (N == 3)
    {
        j = json::array({v.x, v.y, v.z});
    }
    else if constexpr (N == 4)
    {
        j = json::array({v.x, v.y, v.z, v.w});
    }
    else
    {
        static_assert(N >= 2 && N <= 4);
    }

    return;
}

template<length_t N, typename T, qualifier Q>
void from_json(json const& j, vec<N,T,Q>& p)
{
    typedef vec<N,T,Q> U;

    Serde::JsonExpectType<U>(j, json::value_t::array);
    Serde::JsonExpectSize<U>(j, N);

    if constexpr (N == 2)
    {
        p.x = j.at(0).get<T>();
        p.y = j.at(1).get<T>();
    }
    else if constexpr (N == 3)
    {
        p.x = j[0].get<T>();
        p.y = j[1].get<T>();
        p.z = j[2].get<T>();
    }
    else if constexpr (N == 4)
    {
        p.x = j[0].get<T>();
        p.y = j[1].get<T>();
        p.z = j[2].get<T>();
        p.w = j[3].get<T>();
    }
     else
     {
         static_assert(N >= 2 && N <= 4);
     }

     return;
}

} /* ~Namespace glm */

namespace Json
{

inline constexpr Serde::LJsonKey Object(LStringView Key) noexcept { return {Key, json::value_t::object}; }
inline constexpr Serde::LJsonKey Bool(LStringView Key) noexcept { return {Key, json::value_t::boolean}; }
inline constexpr Serde::LJsonKey String(LStringView Key) noexcept { return {Key, json::value_t::string}; }
inline constexpr Serde::LJsonKey Array(LStringView Key) noexcept { return {Key, json::value_t::array}; }
inline constexpr Serde::LJsonKey Integer(LStringView Key) noexcept { return {Key, json::value_t::number_integer}; }
inline constexpr Serde::LJsonKey UInteger(LStringView Key) noexcept { return {Key, json::value_t::number_unsigned}; }
inline constexpr Serde::LJsonKey Float(LStringView Key) noexcept { return {Key, json::value_t::number_float}; }

inline bool DoesObjectContainKeys(json const& Object, TArray<LStringView> Keys, LString* OutMissingKey = nullptr) noexcept
{
    if (Object.is_object() == false)
    {
        LOG_ERROR(LogSerialization, "Provided json is not an object. Failed to check for keys.")
        return false;
    }

    for (auto const& Key : Keys)
    {
        if (Object.contains(Key) == false)
        {
            return false;
        }
    }

    return true;
}

struct LKeyType
{
    enum Type
    {
        Object,
        Bool,
        String,
        Array,
        Integer,
        UInteger,
        Float,
    };

    LStringView Key;
    Type ExpectedType;
};

enum struct EError
{
    MissingKey,
    InvalidType,
};
inline LStringView LexToString(EError Error) noexcept
{
    switch (Error)
    {
    case EError::MissingKey:  { return "MissingKey"; }
    case EError::InvalidType: { return "InvalidType"; }
    default: { std::unreachable(); }
    }
}

inline bool DoesObjectContainTypeCheckedKeys(json const& Object, TArray<LKeyType> Keys, LString* OutKeyError = nullptr, EError* OutError = nullptr) noexcept
{
    LString DummyKey;
    EError DummyError;
    if (OutKeyError == nullptr)
    {
        OutKeyError = &DummyKey;
    }
    if (OutError == nullptr)
    {
        OutError = &DummyError;
    }

    auto& KeyError{*OutKeyError};
    auto& Error{*OutError};

    if (Object.is_object() == false)
    {
        LOG_ERROR(LogSerialization, "Provided json is not an object. Failed to check for keys.")
        return false;
    }

    for (auto const& Key : Keys)
    {
        if (Object.contains(Key.Key) == false)
        {
            KeyError = Key.Key;
            Error = EError::MissingKey;
            return false;
        }

        switch (Key.ExpectedType)
        {
        case LKeyType::Object:
        {
            if (Object[Key.Key].is_object() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::Bool:
        {
            if (Object[Key.Key].is_boolean() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::String:
        {
            if (Object[Key.Key].is_string() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::Array:
        {
            if (Object[Key.Key].is_array() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::Integer:
        {
            if (Object[Key.Key].is_number_integer() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::UInteger:
        {
            if (Object[Key.Key].is_number_integer() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        case LKeyType::Float:
        {
            if (Object[Key.Key].is_number_float() == false)
            {
                KeyError = Key.Key;
                Error = EError::InvalidType;
                return false;
            }
            break;
        }
        default:
        {
            std::unreachable();
        }
        }

        continue;
    }

    return true;
}

[[noreturn]]
inline void DefaultFail(auto const& Category, auto const& Key, EError Error)
{
    if (Error == EError::InvalidType)
    {
        LOG_FATAL(LogSerialization, "[{}]: Key [{}] is of invalid type. Failed to load.", Category, Key)
    }
    if (Error == EError::MissingKey)
    {
        LOG_FATAL(LogSerialization, "[{}]: Key [{}] is missing. Failed to load.", Category, Key)
    }

    std::unreachable();
}

} /* ~Namespace Json */
