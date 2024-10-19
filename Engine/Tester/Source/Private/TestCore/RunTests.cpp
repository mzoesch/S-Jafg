// Copyright mzoesch. All rights reserved.

#include "TestCore/RunTests.h"

namespace Jafg::Tester
{

EPlatformExit::Type* TestExitCode;

}

void Jafg::Tester::RunTests(EPlatformExit::Type* ExitCode)
{
    ::Jafg::Tester::TestExitCode = ExitCode;

    std::cout << "Running tests." << '\n';

    return;
}
