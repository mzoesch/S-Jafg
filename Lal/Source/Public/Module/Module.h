// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

class LModuleSupervisor;

/** Base class for all modules. */
class LAL_API LModuleBase
{
    friend LModuleSupervisor;

public:

    LModuleBase() = default;
    virtual ~LModuleBase() = default;

protected:

    virtual void LoadModule() = 0;
    virtual void TearDownModule() = 0;
};

#define IMPLEMENT_MODULE(ModuleName) \

}
