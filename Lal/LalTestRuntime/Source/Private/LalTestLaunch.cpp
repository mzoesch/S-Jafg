// Copyright mzoesch. All rights reserved.

#include "CoreAfx.h"

#if !WITH_TESTS
    static_assert(false,
        "The entry of LalTestRuntime must be used in an environment where tests are enabled. "
        "Did you mean to compile the main application? Then try to only compile the Runtime "
        "by adding \"--target Runtime\" to your CMake build command."
        );
#endif /* !WITH_TESTS */

#include "TestCore/TestRunner.h"

//#
//# What to actually test.
//#
//# @see Launch/Launch.cpp
//#
#include "Lal/Lal/Source/Test/TestLal.h"

#include "Lal/Lal/Source/Test/TestLalString.h"

#if PLATFORM_WINDOWS
    i32 WINAPI WinMain(_In_ HINSTANCE hInInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ char* pCmdLine, _In_ i32 nCmdShow)
#else /* PLATFORM_WASM */
    i32 main(i32 ArgC, const char* ArgV[])
#endif /* !PLATFORM_WASM */
{
    if (std::filesystem::is_regular_file("jafg.jafgworkspace") == false)
    {
        std::filesystem::current_path(std::filesystem::canonical(std::filesystem::path{"../../../../.."}));
    }

    if (std::filesystem::is_regular_file("jafg.jafgworkspace"))
    {
        return Jafg::Tester::LTestFramework{}.RunRegisteredTests();
    }

    LOG_FATAL(LogTesting,
        "Failed to find the Jafg engine root directory form [{}].",
        std::filesystem::canonical(std::filesystem::current_path()).string()
        )
}
