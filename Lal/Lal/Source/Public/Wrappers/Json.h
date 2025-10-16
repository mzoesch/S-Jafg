// Copyright mzoesch. All rights reserved.

#pragma once

#if LAL_WITH_CLANG
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-W#warnings"
#endif /* LAL_WITH_CLANG */
#define JSON_NOEXCEPTION
    #include "Engine/Vendor/Json/single_include/nlohmann/json.hpp"
#if LAL_WITH_CLANG
    #pragma clang diagnostic pop
#endif /* LAL_WITH_CLANG */

using json = nlohmann::json;
