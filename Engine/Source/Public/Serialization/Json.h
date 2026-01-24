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
