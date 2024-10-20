// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"

PRAGMA_FOR_JAFG_BUILD_TOOL("IncludeAllModuleTests")

namespace Jafg::Tester
{

struct LTestFramework;

TESTER_API extern LTestFramework* GTestFramework;

void TESTER_API RunTests(EPlatformExit::Type* ExitCode);

struct TESTER_API LTestFramework
{
    LTestFramework();
    LTestFramework(const LTestFramework& _) = delete;
    LTestFramework(LTestFramework&& _)      = delete;
    ~LTestFramework();

    FORCEINLINE auto HasErrors() const -> bool { return this->OccuriedErrors > 0; }
    FORCEINLINE auto OnError() const -> void { ++this->OccuriedErrors; }

private:

    mutable int32 OccuriedErrors = 0;
};

} /* Namespace Jafg::Tester */
