// Copyright mzoesch. All rights reserved.

#pragma once

#include "Core/CoreDefines.h"

/**
 * Only for modules that cannot load the engine module.
 * If your module **___CAN LOAD THE ENGINE___** module, please interact with the engine
 * directly and **___DO NOT USE___** this header.
 */
#if PREPROCESSOR_EXCLUDE_FF
#endif /* 0 */

namespace Jafg::EngineForward
{

ENGINEFORWARD_API extern bool         bForwardExitRequest;
ENGINEFORWARD_API extern int32        ForwardCustomExitStatus;
ENGINEFORWARD_API extern LString      ForwardCustomExitReason;

} /* ~Namespace Jafg::EngineForward */

/**
 * Request the engine to exit with a custom status and reason.
 * The engine will exit at the next opportunity.
 *
 * @param Level  The exit status level. INDEX_NONE if no status.
 * @param Reason The exit reason. Empty string if no reason.
 */
#define JAFG_ENGINE_FORWARD_REQUEST_EXIT(Level, Reason)              \
    {                                                                \
        if (::Jafg::EngineForward::bForwardExitRequest)              \
        {                                                            \
            ;                                                        \
        }                                                            \
        else                                                         \
        {                                                            \
            ::Jafg::EngineForward::bForwardExitRequest     = true;   \
            ::Jafg::EngineForward::ForwardCustomExitStatus = Level;  \
            ::Jafg::EngineForward::ForwardCustomExitReason = Reason; \
        }                                                            \
    }
