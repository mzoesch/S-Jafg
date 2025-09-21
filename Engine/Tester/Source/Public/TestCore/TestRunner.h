// Copyright mzoesch. All rights reserved.

#pragma once

#include "Lal.afx"

#if !WITH_TESTS
    #error "Test header file included, but tests are not active."
#endif /* !WITH_TESTS */

#include "TestInclude.h"

namespace Jafg::Tester
{

struct LTestFramework;

struct LTestFramework
{
#if LAL_WITH_CLANG
    typedef std::chrono::time_point<std::chrono::system_clock> LTimePoint;
#else /* LAL_WITH_CLANG */
    typedef std::chrono::time_point<std::chrono::steady_clock> LTimePoint;
#endif /* !LAL_WITH_CLANG */

    TESTER_API LTestFramework();
    ~LTestFramework() = default;

    TESTER_API EPlatformExit::Type RunRegisteredTests() const;

    FORCEINLINE constexpr bool HasErrors() const noexcept { return this->OccurredErrors > 0; }
    FORCEINLINE constexpr void OnError() const noexcept { ++this->OccurredErrors; return; }

    FORCEINLINE constexpr void IncreaseChecksRun(const i32 Value) const noexcept { this->ChecksRun += Value; return; }

private:

    LTimePoint  StartTime;
    mutable i32 ChecksRun      { 0 };
    mutable i32 OccurredErrors { 0 };
};

} /* Namespace Jafg::Tester */
