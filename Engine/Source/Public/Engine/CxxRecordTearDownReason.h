// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg::ECxxRecordTearDownReason
{

enum Type
{
    //# Any normal reason.
    Default,

    //# The record is implicitly torn down because its outer is torn down.
    OuterTearDown,

    //# The record is implicitly torn down because its plugin is unloaded.
    PluginUnload,

    //# The record is implicitly torn down because the engine is shutting down.
    EngineShutdown,
};

} /* ~Namespace Jafg::ECxxRecordTearDownReason */
