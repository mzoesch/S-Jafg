// Copyright mzoesch. All rights reserved.

#include "Minimal.afx"

#if JAFG_WITH_TESTS

#include "Core/Test.h"
#include "Stats/Stats.h"

ENGINE_API TArray<TReference<Jafg::LTrivialTestCase>> Jafg::Detail::TrivialTestCases;

Jafg::LTestRunInstance::LTestRunInstance(ETestCategoryFlags Flags)
{
    STAT_CYCLE_FUNCTION()

    if (Flags & ETestCategoryBits::Trivial)
    {
        LOG_INFO(LogTestingFramework, "Running [{}] trivial tests...", Detail::TrivialTestCases.size())

        for (TReference<LTrivialTestCase>& Test: Detail::TrivialTestCases)
        {
            std::print("Launching [{}]...", Test->FullName());
            Test->Run();

            if (Test->Errors())
            {
                std::println(" FAILED ({} tests)", Test->Total());
                for (LString const& Error: Test->HumanReadableErrors())
                {
                    std::println("  {}", Error);
                }
            }
            else
            {
                std::println(" PASSED ({} tests)", Test->Total());
            }

            this->m_Total += Test->Total();
            this->m_Passed += Test->Passed();
        }
    }

    std::println("Finished running [{}] trivial tests ({} tests, {} passed) in {}ms."
        , Detail::TrivialTestCases.size()
        , this->Total()
        , this->Passed()
        , std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - this->Start).count());
}

#endif /* JAFG_WITH_TESTS */
