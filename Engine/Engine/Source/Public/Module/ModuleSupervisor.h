// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg { class LModuleSupervisor; }

namespace Jafg
{

ENGINE_API extern LModuleSupervisor* GModuleSupervisor;

class ENGINE_API LModuleSupervisor final
{
public:

    LModuleSupervisor();
    LModuleSupervisor(const LModuleSupervisor&)            = delete;
    LModuleSupervisor(LModuleSupervisor&&)                 = delete;
    LModuleSupervisor& operator=(const LModuleSupervisor&) = delete;
    LModuleSupervisor& operator=(LModuleSupervisor&&)      = delete;
    ~LModuleSupervisor();
};

} /* ~Namespace Jafg */
