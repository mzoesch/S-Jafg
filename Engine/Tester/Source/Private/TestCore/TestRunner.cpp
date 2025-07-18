// Copyright mzoesch. All rights reserved.

#include "TestCore/TestRunner.h"

Jafg::Tester::LTestFramework::LTestFramework()
{
    this->StartTime = std::chrono::high_resolution_clock::now();

    LOG_INFO(LogTestingFramework, "Started test framework.")

    return;
}

EPlatformExit::Type Jafg::Tester::LTestFramework::RunRegisteredTests() const
{
    LOG_INFO(LogTestingFramework,
        "Waiting for [{}] simple tests to finish...",
        Tester::GetRegisteredTestCases().size()
        )

    for (LSimpleTestCase* SimpleTestCase : Tester::GetRegisteredTestCases())
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
                      << SimpleTestCase->GetNumberOfTestsRun()
                      << " tests." << '\n';

            for (const std::string& Error : SimpleTestCase->GetErrors())
            {
                std::cout << "  " << Error << '\n';
            }

            this->OnError();
        }
        else
        {
            std::cout << " OK. Run "
                      << SimpleTestCase->GetNumberOfTestsRun()
                      << " tests." << '\n';
        }

        this->IncreaseChecksRun(SimpleTestCase->GetNumberOfTestsRun());

        std::cout.flush();

        continue;
    }

    const std::chrono::duration<long long, std::milli> Duration
    {
        std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - this->StartTime)
    };

    LOG_INFO(LogTestingFramework,
        "Run [{}] simple tests with a total of [{}] checks in {}ms." ,
        ::Jafg::Tester::GetRegisteredTestCases().size(),
        this->ChecksRun,
        Duration.count()
        )

    LOG_VERBOSE(LogTestingFramework, "Finished test framework. Tearing down framework...")

    if (this->HasErrors())
    {
        LOG_ERROR(LogTestingFramework, "Test framework encountered [{}] errors.", this->OccurredErrors)
    }
    else
    {
        LOG_INFO(LogTestingFramework, "Test framework encountered no errors.")
    }

    return this->HasErrors() ? EPlatformExit::Error : EPlatformExit::Success;
}

#if WITH_TESTS
TEST_CASE(Types, "Jafg.TestFramework")
{
    CHECK_EQUALS("Test TestFramework",    0,    0)
    CHECK_EQUALS("Test TestFramework", 0.0f, 0.0f)
    CHECK_EQUALS("Test TestFramework", 0.0 , 0.0 )
    CHECK_EQUALS("Test TestFramework", 0.0L, 0.0L)

    return;
}
#endif /* WITH_TESTS */
