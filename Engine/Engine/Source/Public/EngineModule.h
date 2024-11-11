// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"
#include "Module/Module.h"

namespace Jafg
{

class LEngineModule final : public LModuleBase
{
protected:

    // LModuleBase implementation
    void LoadModule() override;
    void TearDownModule() override;
    // ~LModuleBase implementation
};

} /* ~Namespace Jafg */
