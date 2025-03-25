// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg
{

namespace Private
{

//# Called at the beginning of a frame by the engine. This will be the last frame.
void BeginExitIfRequested();

//# @return True, if an exit request was forwarded to the engine.
bool ReflectForwardEngineExitRequest();

} /* ~Namespace Private */

void RequestEngineExit();
void RequestEngineExit(const LSimpleString& Reason);
void RequestEngineExit(const i32 CustomExitStatus);
void RequestEngineExit(const i32 CustomExitStatus, const LSimpleString& Reason);

} /* ~Namespace Jafg */

