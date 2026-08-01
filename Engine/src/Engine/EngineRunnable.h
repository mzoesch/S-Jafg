// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Runnable.h"

namespace Jafg
{

class LEngineRunnable final : public LRunnable
{
public:

    LEngineRunnable() = default;
    LEngineRunnable(const LString& InHumanReadableName)
        : LRunnable(InHumanReadableName)
    {
    }

    virtual ETaskExit::Type Run() override;
};

} /* ~Namespace Jafg */
