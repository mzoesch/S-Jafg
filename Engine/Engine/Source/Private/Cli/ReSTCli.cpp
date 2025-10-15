// Copyright mzoesch. All rights reserved.

#include "Cli/ReSTCli.h"

Jafg::LReStCli::LReStCli() noexcept
    : LRunnable{"ReSTCli"}
{
    return;
}

Jafg::ETaskExit::Type Jafg::LReStCli::Initialize()
{
    LOG_VERBOSE(LogReST, "Initializing ReSTCli thread.")

    return LRunnable::Initialize();
}

Jafg::ETaskExit::Type Jafg::LReStCli::Run()
{
    LOG_VERBOSE(LogReST, "Starting ReSTCli thread.")
    return ETaskExit::Success;
}

void Jafg::LReStCli::Exit()
{
    LOG_INFO(LogReST, "Exiting ReSTCli thread.")
    LRunnable::Exit();

    return;
}

void Jafg::LReStCli::OnStop(const ERunnableStopReason::Type InType)
{
    LOG_VERBOSE(LogReST, "Stopping ReSTCli thread.")
    LRunnable::OnStop(InType);

    return;
}
