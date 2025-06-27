// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX

#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <cstring>

void Lal::LOnPlatformBreakLinux::OnProgramPanicImpl()
{
    abort();
}

void Lal::LOnPlatformBreakLinux::OnProgramPanic
(
    const LPrimitivePlatformTypesGeneric::LChar* InMessage,
    const LPrimitivePlatformTypesGeneric::LChar* InFile,
    const LPrimitivePlatformTypesGeneric::u64    InLine
)
{
    OnProgramPanicImpl();
}

namespace Lal::Hal
{

void SleepNoStats(const f64 InSeconds)
{
    if (const i32 Milli { static_cast<i32>(InSeconds * JAFG_S2MUS_D) }; Milli > 0)
    {
        usleep(Milli);
    }
    else
    {
        YieldThread();
    }

    return;
}

void YieldThread()
{
    sched_yield();
    return;
}

//
// Note that this function only currently works with GDB.
//
bool IsTracerPidValidVerySlow()
{
    char Buffer[4096];

    const i32 Fd { ::open("/proc/self/status", O_RDONLY) };
    if (Fd == -1)
    {
        return false;
    }

    const ssize_t NumRead { ::read(Fd, Buffer, sizeof(Buffer) - 1) };
    ::close(Fd);

    if (NumRead <= 0)
    {
        return false;
    }

    Buffer[NumRead] = '\0';
    constexpr char PidTracerString[] { "TracerPid:" };
    const char* PidTracer { ::strstr(Buffer, PidTracerString) };
    if (!PidTracer)
    {
        return false;
    }

    for (const char* CharacterPtr { PidTracer + sizeof(PidTracerString) - 1 }; CharacterPtr <= Buffer + NumRead; ++CharacterPtr)
    {
        if (::isspace(*CharacterPtr))
        {
            continue;
        }

        return ::isdigit(*CharacterPtr) != 0 && *CharacterPtr != '0';
    }

    return false;
}

} /* ~Namespace Lal::Hal */

#endif /* PLATFORM_LINUX */
