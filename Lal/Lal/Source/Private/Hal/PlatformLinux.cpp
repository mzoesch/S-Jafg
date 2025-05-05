// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include <unistd.h>

void LLinuxPlatformBreakDefines::OnProgramPanic(const std::string& InMessage, const std::string& InFile, const LLinuxPlatformTypes::i32 InLine)
{
    // TODO Error popup.
    LOG_ERROR(LogJafgInternal, "[{}] in [{}] at [{}]", InMessage, InFile, InLine);
    abort();
}

namespace Jafg::PlatformHal
{

void SleepNoStats(const f64 InSeconds)
{
    if (const i32 Milli = static_cast<i32>(InSeconds * JAFG_S2MUS_D); Milli > 0)
    {
        usleep(Milli);
    }
    else
    {
        sched_yield();
    }

    return;
}

void YieldThread()
{
    sched_yield();
    return;
}

} /* ~Namespace Jafg::PlatformHal */

#endif /* PLATFORM_LINUX */
