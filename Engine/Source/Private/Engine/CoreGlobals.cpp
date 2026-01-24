// Copyright mzoesch. All rights reserved.

#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"

void Jafg::Private::BeginExitIfRequested()
{
    if (IsEngineExitRequested())
    {
        bGEngineRequestingExit = true;
    }

    return;
}

// TODO Make threadsafe
void Jafg::RequestEngineExit()
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit = true;

    return;
}

void Jafg::RequestEngineExit(const LString& Reason)
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit = true;
    GCustomExitReason   = Reason;

    return;
}

void Jafg::RequestEngineExit(const i32 CustomExitStatus)
{
    if (WillShortlyTerminate())
    {
        return;
    }

    bGShouldRequestExit       = true;
    GCustomExitStatusOverride = CustomExitStatus;

    return;
}

void Jafg::RequestEngineExit(const i32 CustomExitStatus, const LString& Reason)
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
