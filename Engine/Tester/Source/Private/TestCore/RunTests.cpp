// Copyright mzoesch. All rights reserved.

#include "TestCore/RunTests.h"
#if WITH_TESTS
    /* Order *must* be preserved. */
    #include "TestInclude.h"
#endif /* WITH_TESTS */
#include "TesterInclude.h"

#if WITH_TESTS
    /*-----------------------------------------------------------------------------
        Do not remove this seemingly useless include.
        When we include this file, all the test cases will be statically
        initialized and transitively added to the list of test cases.
    -----------------------------------------------------------------------------*/
    #include "RunTests.generated.h"
#endif /* WITH_TESTS */

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
            std::cout << " FAILED. Run "
                      << SimpleTestCase->GetNumberOfRunTests()
                      << " tests." << '\n';

            for (const std::string& Error : SimpleTestCase->GetErrors())
            {
                std::cout << "  " << Error << '\n';
            }

            GTestFramework->OnError();
        }
        else
        {
            std::cout << " OK. Run "
                      << SimpleTestCase->GetNumberOfRunTests()
                      << " tests." << '\n';
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
    std::cout << "Finished running all tests. Tearing down framework ..." << '\n';
    std::cout << "Test framework encountered "
              << this->OccuriedErrors
              << " errors." << '\n';

    *::Jafg::Tester::TestExitCode = this->HasErrors() ? EPlatformExit::Error : EPlatformExit::Success;

    return;
}

#if WITH_TESTS
TEST_CASE(Types, "Jafg.TestFramework")
{
    CHECK_EQUALS("Test the TestFramework",    0,    0)
    CHECK_EQUALS("Test the TestFramework", 0.0f, 0.0f)
    CHECK_EQUALS("Test the TestFramework", 0.0 , 0.0 )
    CHECK_EQUALS("Test the TestFramework", 0.0L, 0.0L)

    return;
}
#endif /* WITH_TESTS */
