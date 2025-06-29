// Copyright mzoesch. All rights reserved.

#pragma once

#include "Async/Runnable.h"

namespace Jafg
{

class LEngineRunnable final : public LRunnable
{
public:

    ENGINE_API LEngineRunnable();
    ENGINE_API LEngineRunnable(const LString& InHumanReadableName);

    virtual ETaskExit::Type Run() override;
};

} /* ~Namespace Jafg */
