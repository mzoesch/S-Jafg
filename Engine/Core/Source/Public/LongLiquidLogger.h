// Copyright mzoesch. All rights reserved.

#pragma once

namespace JafgCore
{

#if LAL_SAVE_LOGS_IN_MEMORY
CORE_API extern std::vector<std::tuple<
    std::string, /* Verbosity */
    std::string  /* Message */
    >> GLongLiquidLogs;
#endif /* LAL_SAVE_LOGS_IN_MEMORY */

} /* ~Namespace JafgCore */
