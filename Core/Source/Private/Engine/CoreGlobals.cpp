// Copyright mzoesch. All rights reserved.

#include "CoreAFX.h"
#include "CoreGlobals.h"
#include "Engine/Engine.h"

void BeginExitIfRequested()
{
    if (IsEngineExitRequested())
    {
        bGEngineRequestingExit = true;
    }

    return;
}

void RequestEngineExit()
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit = true;

    return;
}

void RequestEngineExit(const String& Reason)
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit = true;
    GCustomExitReason   = Reason;

    return;
}

void RequestEngineExit(const int32 CustomExitStatus)
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit       = true;
    GCustomExitStatusOverride = CustomExitStatus;

    return;
}

void RequestEngineExit(const int32 CustomExitStatus, const String& Reason)
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit       = true;
    GCustomExitStatusOverride = CustomExitStatus;
    GCustomExitReason         = Reason;

    return;
}
