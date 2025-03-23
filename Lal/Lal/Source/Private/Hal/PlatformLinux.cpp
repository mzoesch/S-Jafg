// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include <unistd.h>

void LLinuxPlatformBreakDefines::OnProgramPanic(const std::string& InMessage, const std::string& InFile, const LLinuxPlatformTypes::int32 InLine)
{
    // TODO Error popup.
    LOG_ERROR(LogJafgInternal, "[{}] in [{}] at [{}]", InMessage, InFile, InLine);
    abort();
}

namespace PlatformHal
{

void Sleep(const double InSeconds)
{
    if (const int32 Milli = static_cast<int32>(InSeconds * JAFG_S2MUS_D); Milli > 0)
    {
        usleep(Milli);
    }
    else
    {
        sched_yield();
    }

    return;
}

} /* ~Namespace PlatformHal */

#endif /* PLATFORM_LINUX */
