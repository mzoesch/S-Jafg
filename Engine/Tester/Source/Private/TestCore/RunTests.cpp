// Copyright mzoesch. All rights reserved.

#include "TestCore/RunTests.h"
#include "TesterInclude.h"

/*-----------------------------------------------------------------------------
    Do not remove this seemingly useless include.
    When we include this file, all the test cases will be statically
    initialized and transitively added to the list of test cases.
-----------------------------------------------------------------------------*/
#include "RunTests.generated.h"

namespace Jafg::Tester
{

extern std::vector<::Jafg::Tester::LSimpleTestCase*>& GetAllSimpleTestCasesDuringStaticInitialization( void );

EPlatformExit::Type* TestExitCode;

TESTER_API LTestFramework* GTestFramework = nullptr;

} /* Namespace Jafg::Tester */

void Jafg::Tester::RunTests(EPlatformExit::Type* ExitCode)
{
    ::Jafg::Tester::TestExitCode = ExitCode;

    GTestFramework = new LTestFramework();

    std::cout << "Starting simple tests ..." << '\n';
    std::cout << "Waiting for "
              << ::Jafg::Tester::GetAllSimpleTestCasesDuringStaticInitialization().size()
              << " simple tests to finish ..." << '\n';
    std::cout.flush();

    for (::Jafg::Tester::LSimpleTestCase*& SimpleTestCase : ::Jafg::Tester::GetAllSimpleTestCasesDuringStaticInitialization())
    {
        if (SimpleTestCase == nullptr)
        {
            continue;
        }

        std::cout << "Starting simple test ["
                  << SimpleTestCase->GetFullName()
                  << "] ...";

        SimpleTestCase->Run();

        if (SimpleTestCase->HasErrors())
        {
            std::cout << " FAILED" << '\n';

            for (const std::string& Error : SimpleTestCase->GetErrors())
            {
                std::cout << "  " << Error << '\n';
            }

            GTestFramework->OnError();
        }
        else
        {
            std::cout << " OK" << '\n';
        }

        std::cout.flush();

        continue;
    }

    delete GTestFramework;
    GTestFramework = nullptr;

    return;
}

Jafg::Tester::LTestFramework::LTestFramework()
{
    std::cout << "Started test framework." << '\n';

    return;
}

Jafg::Tester::LTestFramework::~LTestFramework()
{
    std::cout << "Finished running tests. Tearing down framework." << '\n';
    std::cout << "Test framework encountered "
              << this->OccuriedErrors
              << " errors." << '\n';

    *::Jafg::Tester::TestExitCode = this->HasErrors() ? EPlatformExit::Error : EPlatformExit::Success;

    return;
}

TEST_CASE(TestTheTestframework, "Jafg.TestFramework")
{
    CHECK_EQUALS("Test the TestFramework", 1, 0)

    return;
}
