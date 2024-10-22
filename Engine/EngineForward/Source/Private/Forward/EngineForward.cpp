// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Forward/EngineForward.h"

namespace Jafg::EngineForward
{

ENGINEFORWARD_API bool         bForwardExitRequest        = false;
ENGINEFORWARD_API int32        ForwardCustomExitStatus    = INDEX_NONE;
ENGINEFORWARD_API LString      ForwardCustomExitReason    = "";

} /* ~Namespace Jafg::EngineForward */
