// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

namespace Jafg
{

namespace Private
{

/** Called at the beginning of a frame by the engine. This will be the last frame. */
void BeginExitIfRequested();

/** @return True, if an exit request was forwarded to the engine. */
bool ReflectForwardEngineExitRequest();

} /* ~Namespace Private */

void RequestEngineExit();
void RequestEngineExit(const LString& Reason);
void RequestEngineExit(const int32 CustomExitStatus);
void RequestEngineExit(const int32 CustomExitStatus, const LString& Reason);

} /* ~Namespace Jafg */
