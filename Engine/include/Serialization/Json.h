// Copyright mzoesch. All rights reserved.

#pragma once

namespace serde
{

template<typename T>
[[noreturn]]
void NlohmannSink(T const& Error) noexcept;

} /* ~Namespace serde */

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
#endif /* JAFG_WITH_CLANG */
    #define JSON_NOEXCEPTION
    #define JSON_THROW_USER                             ::serde::NlohmannSink
    #define JSON_DISABLE_ENUM_SERIALIZATION             1
    #include "nlohmann/json.hpp"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

using json = nlohmann::json;

//# Do not use enum, as they can silently fail.
#undef NLOHMANN_JSON_SERIALIZE_ENUM

//# Do not use these, as they are not as powerful as our sad macro utopia.
#undef NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE
#undef NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT
#undef NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_ONLY_SERIALIZE

//#
//# Same as SERDE_ENUM, but comes also with JSON support.
//# Usage example:
//#     enum struct EExample: u8 { A, B, C };
//#     SERDE_JSON_ENUM(EExample, A, B, C)
//#
#define SERDE_JSON_ENUM(T, ...) SERDE_ENUM(T, __VA_ARGS__) SERDE_JSON_ENUM_IMPL(T)
//#
//# Same as SERDE_ENUM_PAIR, but comes also with JSON support.
//# Usage example:
//#     enum struct EExample: u8 { A, B, C };
//#     SERDE_JSON_ENUM(EExample, {
//#         {EExample::A, "a"}, {EExample::B, "b"}, {EExample::C, "c"}
//#         })
#define SERDE_JSON_ENUM_PAIR(T, ...) SERDE_ENUM_PAIR(T, __VA_ARGS__) SERDE_JSON_ENUM_IMPL(T)
#define SERDE_JSON_ENUM_IMPL(T) SERDE_JSON_ENUM_ONLY_SERIALIZE(T) SERDE_JSON_ENUM_ONLY_DESERIALIZE(T)
#define SERDE_JSON_ENUM_ONLY_SERIALIZE(T) \
    template<typename BasicJsonType>                                                            \
    inline void to_json(BasicJsonType& j, const T& e)                                   \
    {                                                                                           \
        static_assert(std::is_enum_v<T>);          \
        std::unordered_map<T, LStringView> const& Members{::serde::enum_map<T>()}; \
        auto it = std::find_if(std::begin(Members), std::end(Members),                                      \
                               [e](const std::pair<T, BasicJsonType>& ej_pair) -> bool  \
        {                                                                                       \
            return ej_pair.first == e;                                                          \
        });                                                                                     \
        if (it == std::end(Members)) \
        { \
            LOG_FATAL(LogSerialization, "Failed to serialize enum [{}] to JSON. No matching string found for value [{}]." \
                , algo::type_name<T>(), std::to_underlying(e)) \
        } \
        else \
        { \
            j = it->second; \
        } \
    }
#define SERDE_JSON_ENUM_ONLY_DESERIALIZE(T) \
    template<typename BasicJsonType>                                                            \
    inline void from_json(const BasicJsonType& j, T& e)                                \
    {                                                                                           \
        static_assert(std::is_enum_v<T>);          \
        std::unordered_map<T, LStringView> const& Members{::serde::enum_map<T>()}; \
        auto it = std::find_if(std::begin(Members), std::end(Members),                                      \
                               [&j](const std::pair<T, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        }); \
        if (it == std::end(Members)) \
        { \
            LOG_FATAL(LogSerialization, "Failed to deserialize enum [{}] from JSON string [{}]." \
                , algo::type_name<T>(), j.template get<LString>()) \
        } \
        else \
        { \
            e = it->first; \
        } \
    }

//#
//# Serializes an enum where its string counterpart is different from the C++ record name.
//# Usage example:
//#     enum struct EExample: u8 { A, B, C };
//#     SERDE_JSON_ENUM(EExample, {
//#         {EExample::A, "a"}, {EExample::B, "b"}, {EExample::C, "c"}
//#         })
//#
#define SERDE_JSON_ENUM_PAIR_ONLY_DESERIALIZE(T, ...) \
    template<typename BasicJsonType>                                                            \
    inline void from_json(const BasicJsonType& j, T& e)                                 \
    {                                                                                           \
        static_assert(std::is_enum_v<T>);          \
        static constexpr std::pair<T, LStringView> const Members[]{ \
        __VA_ARGS__ \
        }; \
        auto it = std::find_if(std::begin(Members), std::end(Members),                                      \
                               [&j](const std::pair<T, BasicJsonType>& ej_pair) -> bool \
        {                                                                                       \
            return ej_pair.second == j;                                                         \
        }); \
        if (it == std::end(Members)) \
        { \
            LOG_FATAL(LogSerialization, "Failed to deserialize enum [{}] from JSON string [{}]." \
                , algo::type_name<T>(), j.template get<LString>()) \
        } \
        else \
        { \
            e = it->first; \
        } \
    }

//#
//# Serializes a type to/from JSON.
//# Usage example:
//#    struct LExample{ LString Name; i32 Value; };
//#    SERDE_JSON_TYPE(LExample, Name, Value)
//#
#define SERDE_JSON_TYPE(Type, ...)  \
    SERDE_JSON_TYPE_ONLY_SERIALIZE(Type, __VA_ARGS__) \
    SERDE_JSON_TYPE_ONLY_DESERIALIZE(Type, __VA_ARGS__)
#define SERDE_JSON_TYPE_ONLY_SERIALIZE(Type, ...)  \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void to_json(BasicJsonType& nlohmann_json_j, Type const& nlohmann_json_t) { JAFG_MAP(NLOHMANN_JSON_TO, __VA_ARGS__) }
#define SERDE_JSON_TYPE_ONLY_DESERIALIZE(Type, ...)  \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void from_json(BasicJsonType const& nlohmann_json_j, Type& nlohmann_json_t) { JAFG_MAP(NLOHMANN_JSON_FROM, __VA_ARGS__) }
//#
//# Serialize a type to JSON which has unique logic.
//# Usage example:
//#     struct LExample{ ... };
//#     SERDE_JSON_TYPE_ONLY_SERIALIZE_BODY(LExample)
//#     {
//#         j = ...; /* Your logic here. */
//#     }
//#
#define SERDE_JSON_TYPE_ONLY_SERIALIZE_BODY(Type) \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void to_json(BasicJsonType& j, Type const& field)
//#
//# Serialize a type from JSON to a unique data layout.
//# Usage example:
//#     struct LExample{ ... };
//#     SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(LExample)
//#     {
//#         field = ...; /* Your logic here. */
//#     }
//#
#define SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(Type)  \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void from_json(BasicJsonType const& j, Type& field)

//#
//# Serializes a type to/from JSON but if the fields are missing, the filed will be skipped and the current value will
//# be left untouched.
//#
//# @warning This is very different from the nlohmann DEFAULT macro helper, that would default assign the missing
//#          fields.
//#
//# Usage example:
//#    struct LExample{ LString Name; i32 Value; };
//#    SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE(LExample, Name, Value)
//#
#define SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE(Type, ...)  \
    SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE_ONLY_SERIALIZE(Type, __VA_ARGS__) \
    SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE_ONLY_DESERIALIZE(Type, __VA_ARGS__)
#define DETAIL_SERDE_JSON_TYPE_RELAXED(v1) \
    if (nlohmann_json_j.contains(#v1)) \
    { \
        NLOHMANN_JSON_FROM(v1) \
    }
#define SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE_ONLY_SERIALIZE(Type, ...) \
    SERDE_JSON_TYPE_ONLY_SERIALIZE(Type, __VA_ARGS__)
#define SERDE_JSON_TYPE_RELAXED_NON_INTRUSIVE_ONLY_DESERIALIZE(Type, ...)  \
    template<typename BasicJsonType, nlohmann::detail::enable_if_t<nlohmann::detail::is_basic_json<BasicJsonType>::value, int> = 0> \
    void from_json(const BasicJsonType& nlohmann_json_j, Type& nlohmann_json_t) { const Type nlohmann_json_default_obj{}; JAFG_MAP(DETAIL_SERDE_JSON_TYPE_RELAXED, __VA_ARGS__) }

NLOHMANN_JSON_NAMESPACE_BEGIN
namespace detail
{

SERDE_ENUM_PAIR(value_t, {
    {json::value_t::null, "null"},
    {json::value_t::object, "object"},
    {json::value_t::array, "array"},
    {json::value_t::string, "string"},
    {json::value_t::boolean, "boolean"},
    {json::value_t::binary, "binary"},
    {json::value_t::discarded, "discarded"},
    {json::value_t::number_integer, "number"},
    {json::value_t::number_unsigned, "number"},
    {json::value_t::number_float, "number"},
    })

} /* ~Namespace nlohmann */
NLOHMANN_JSON_NAMESPACE_END

namespace serde
{

template<typename T>
[[noreturn]]
inline void NlohmannSink(T const& Error) noexcept
{
    LOG_FATAL(LogSerialization, "{}", Error.what())
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
        JsonSink<T>(j, "Expected [{}] got [{}]", serde::to_string(Type), j.type_name());
    }
}
template<typename T>
inline void JsonExpectSize(json const& j, std::size_t Size) noexcept
{
    if (j.size() != Size)
    {
        JsonSink<T>(j, "Expected size [{}] got [{}]", Size, j.size());
    }
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

} /* ~Namespace serde */

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

    serde::JsonExpectType<U>(j, json::value_t::array);
    serde::JsonExpectSize<U>(j, N);

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

SERDE_JSON_TYPE_ONLY_SERIALIZE_BODY(LColor)
{
    j = json::string_t{serde::to_string(field)};
}
SERDE_JSON_TYPE_ONLY_DESERIALIZE_BODY(LColor)
{
    serde::from_string(&field, j.template get<LString>());
}
