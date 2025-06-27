// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAfx.h"

namespace Jafg::Tester
{

struct LTestFramework;

TESTER_API extern LTestFramework* GTestFramework;

void TESTER_API RunTests(EPlatformExit::Type* ExitCode);

struct TESTER_API LTestFramework
{
#if LAL_WITH_CLANG
    typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;
#else /* LAL_WITH_CLANG */
    typedef std::chrono::time_point<std::chrono::steady_clock> TimePoint;
#endif /* !LAL_WITH_CLANG */

    LTestFramework();
    LTestFramework(const LTestFramework& _) = delete;
    LTestFramework(LTestFramework&& _)      = delete;
    ~LTestFramework();

    FORCEINLINE auto HasErrors() const -> bool { return this->OccuriedErrors > 0; }
    FORCEINLINE auto OnError() const -> void { ++this->OccuriedErrors; }

    FORCEINLINE auto IncreaseRunChecks(const i32 Value) const -> void { this->RunChecks += Value; }

private:

    TimePoint     StartTime;
    mutable i32 RunChecks      = 0;
    mutable i32 OccuriedErrors = 0;
};

} /* Namespace Jafg::Tester */
