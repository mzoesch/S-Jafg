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
        Detail::EmitAndFlushLogs();

        if (!Detail::TrivialTestCases.empty())
        {
            std::stringstream SS;
            for (TReference<LTrivialTestCase>& Test: Detail::TrivialTestCases)
            {
                SS << algo::sprintf("Launching [{}]...", Test->FullName());
                Test->Run();

                if (Test->Errors())
                {
                    SS << algo::sprintf(" FAILED ({} tests)\n", Test->Total());
                    for (LString const& Error: Test->HumanReadableErrors())
                    {
                        SS << algo::sprintf("  {}\n", Error);
                    }
                }
                else
                {
                    SS << algo::sprintf(" PASSED ({} tests)\n", Test->Total());
                }

                this->m_Total += Test->Total();
                this->m_Passed += Test->Passed();
            }
            auto Str{SS.str()};
            if (!Str.empty())
            {
                Str.pop_back();
            }
            LOG_INFO(LogTestingFramework, "\n{}", Str)
        }
    }

    LOG_INFO(LogTestingFramework, "Finished running [{}] trivial tests ({} tests, {} passed) in {}ms."
        , Detail::TrivialTestCases.size()
        , this->Total()
        , this->Passed()
        , std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - this->Start).count()
        )
}

#endif /* JAFG_WITH_TESTS */
