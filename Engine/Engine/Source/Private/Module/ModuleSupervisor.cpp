// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "Module/ModuleSupervisor.h"

namespace Jafg
{

ENGINE_API LModuleSupervisor* GModuleSupervisor = nullptr;

LModuleSupervisor::LModuleSupervisor()
{
    if (GModuleSupervisor)
    {
        panic("The global module supervisor is already set.")
    }

    GModuleSupervisor = this;
    return;
}

LModuleSupervisor::~LModuleSupervisor()
{
    if (GModuleSupervisor == this)
    {
        check( GModuleSupervisor == this )
        GModuleSupervisor = nullptr;
    }

    return;
}

} /* ~Namespace Jafg */
