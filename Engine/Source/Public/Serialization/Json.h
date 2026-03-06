// Copyright mzoesch. All rights reserved.

#pragma once

#if JAFG_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
#endif /* JAFG_WITH_CLANG */
#define JSON_NOEXCEPTION
    #include "nlohmann/json.hpp"
#if JAFG_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* JAFG_WITH_CLANG */

using json = nlohmann::json;

namespace Json
{

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
    default: { unreachable() }
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
            unreachable()
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

    unreachable()
}

} /* ~Namespace Json */
