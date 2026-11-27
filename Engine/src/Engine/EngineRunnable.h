// Copyright mzoesch. All rights reserved.

#pragma once

#include "Framework/Runnable.h"

namespace Jafg
{

class LEngineRunnable final: public LRunnable
{
public:

    LEngineRunnable() = delete;
    LEngineRunnable(LString InHumanReadableName): LRunnable{std::move(InHumanReadableName)} {}

    Tasks::EExit::Type Run(std::stop_token const& Stoken) override;
};

} /* ~Namespace Jafg */
