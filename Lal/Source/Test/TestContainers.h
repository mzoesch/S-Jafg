// Copyright mzoesch. All rights reserved.

#pragma once

#include "CoreAFX.h"
#include "TestInclude.h"

TEST_CASE(SimpleIntegerArrayOperations, "Lal.Containers")
{
    using namespace Jafg;

    TdhArray<int> Arr;

    CHECK_EQUALS("Arr zero size.", Arr.GetSize(), 0)

    return;
}
