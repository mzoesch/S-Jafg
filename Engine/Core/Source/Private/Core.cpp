// Copyright mzoesch. All rights reserved.

#include "Specs.h"
#include "RegisteredProgramArguments.h"
#include <vector>
#include <string>

namespace JafgCore
{

CORE_API bool bGAlreadyCrashed{ false };
CORE_API bool bGSuppressCrashDialog{ false };
CORE_API std::vector<Lal::LProgramParameter*> GRegisteredProgramArguments;

/* This still exists even if LAL_SAVE_LOGS_IN_MEMORY but how would we access that macro here?? */
CORE_API std::vector<std::tuple<std::string, std::string>> GLongLiquidLogs;

} /* ~Namespace JafgCore */
