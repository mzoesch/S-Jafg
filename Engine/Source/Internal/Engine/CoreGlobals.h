// Copyright mzoesch. All rights reserved.

#pragma once

#include "Minimal.afx"

namespace Jafg
{

namespace Private
{

//# Called at the beginning of a frame by the engine. This will be the last frame.
void BeginExitIfRequested();

} /* ~Namespace Private */

void RequestEngineExit();
void RequestEngineExit(const LString& Reason);
void RequestEngineExit(const i32 CustomExitStatus);
void RequestEngineExit(const i32 CustomExitStatus, const LString& Reason);

} /* ~Namespace Jafg */

