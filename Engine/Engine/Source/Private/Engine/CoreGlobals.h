// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace Private
{

void BeginExitIfRequested();

}

void RequestEngineExit();
void RequestEngineExit(const LString& Reason);
void RequestEngineExit(const int32 CustomExitStatus);
void RequestEngineExit(const int32 CustomExitStatus, const LString& Reason);

}
