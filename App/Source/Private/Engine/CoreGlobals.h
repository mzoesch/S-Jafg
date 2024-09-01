// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

void BeginExitIfRequested();

void RequestEngineExit();
void RequestEngineExit(const String& Reason);
void RequestEngineExit(const int32 CustomExitStatus);
void RequestEngineExit(const int32 CustomExitStatus, const String& Reason);
