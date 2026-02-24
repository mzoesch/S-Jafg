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

} /* ~Namespace Json */
