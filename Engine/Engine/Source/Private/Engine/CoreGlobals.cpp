// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"
#include "Engine/CoreGlobals.h"
#include "Engine/Engine.h"
#include "Forward/EngineForward.h"

void Jafg::Private::BeginExitIfRequested()
{
    if (IsEngineExitRequested() || ReflectForwardEngineExitRequest())
    {
        bGEngineRequestingExit = true;
    }

    return;
}

bool Jafg::Private::ReflectForwardEngineExitRequest()
{
    if (::Jafg::EngineForward::bForwardExitRequest)
    {
        ::Jafg::RequestEngineExit(
            ::Jafg::EngineForward::ForwardCustomExitStatus,
            ::Jafg::EngineForward::ForwardCustomExitReason
        );

        return true;
    }

    return false;
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

void Jafg::RequestEngineExit(const LSimpleString& Reason)
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

void Jafg::RequestEngineExit(const i32 CustomExitStatus, const LSimpleString& Reason)
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
