// Copyright mzoesch. All rights reserved.

#pragma once

#if !WITH_TESTS
    #error "Test header file included, but tests are not active."
#endif /* !WITH_TESTS */


/*----------------------------------------------------------------------------
    Commonly used headers.
----------------------------------------------------------------------------*/

#include "TestCore/TestCase.h"
#include "TestCore/TestMacros.h"

#ifndef TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS
    #define TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS         0
#endif /* TESTER_DO_INCLUDE_LEGACY_UNIT_TESTS */
