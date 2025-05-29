// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(Optional, "Lal.Containers")
{
    using namespace Jafg;

    TOptional<int> A;
    TOptional<int> B;

    QUICK_CHECK_FALSE( static_cast<bool>(A) )
    QUICK_CHECK_FALSE( static_cast<bool>(B) )

    A = 6;

    QUICK_CHECK_TRUE(  static_cast<bool>(A) )
    QUICK_CHECK_FALSE( static_cast<bool>(B) )

    QUICK_CHECK_FALSE( A == B )
    QUICK_CHECK_FALSE( B == A )
    QUICK_CHECK_TRUE(  A == A )
    QUICK_CHECK_TRUE(  B == B )

    B = 6;
    QUICK_CHECK_TRUE(  A == B )
    QUICK_CHECK_TRUE(  B == A )
    QUICK_CHECK_TRUE(  A == A )
    QUICK_CHECK_TRUE(  B == A )

    A = 5;
    QUICK_CHECK_FALSE( A == B )
    QUICK_CHECK_FALSE( B == A )
    QUICK_CHECK_TRUE(  A == A )
    QUICK_CHECK_TRUE(  B == B )

    return;
}
