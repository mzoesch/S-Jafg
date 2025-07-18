// Copyright mzoesch. All rights reserved.

#if PLATFORM_LINUX
    #ifndef LAL_PLATFORM_CALLSPEC_IN
        #define LAL_PLATFORM_CALLSPEC_IN        __attribute__ ((visibility ("default")))
    #endif /* !LAL_PLATFORM_CALLSPEC_IN */
    #ifndef LAL_PLATFORM_CALLSPEC_OUT
        #define LAL_PLATFORM_CALLSPEC_OUT       __attribute__ ((visibility ("default")))
    #endif /* !LAL_PLATFORM_CALLSPEC_OUT */
#else /* PLATFORM_LINUX */
    #error "Missing implementation for this PLATFORM."
#endif /* PLATFORM_LINUX */

#include "TestCore/TestCase.h"

namespace Jafg::Tester
{

const std::vector<LSimpleTestCase*>& GetRegisteredTestCases()
{
    return Private::GetMutableRegisteredTestCases();
}

namespace Private
{

std::vector<LSimpleTestCase*>& GetMutableRegisteredTestCases()
{
    static std::vector<LSimpleTestCase*> RegisteredCases;
    return RegisteredCases;
}

} /* ~Namespace Private */

} /* ~Namespace Jafg::Tester */
