// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

namespace Jafg::Testing::Smart
{

struct LMySmartCounter
{
    LMySmartCounter() = delete;
    explicit LMySmartCounter(i32* InCounter) : Counter(InCounter) { ++*this->Counter; }
    LMySmartCounter(const LMySmartCounter& InOther) : Counter(InOther.Counter) { ++*this->Counter; }
    LMySmartCounter(LMySmartCounter&& InOther) noexcept : Counter(InOther.Counter) { InOther.Counter = nullptr; }
    ~LMySmartCounter() { check( this->Counter)  ++*this->Counter; }

    i32* Counter = nullptr;
};

} /* Namespace Jafg::Testing::Smart */

TEST_CASE(SmartUniqueOperations, "Lal.Smart")
{
    using namespace Jafg;
    using namespace Jafg::Testing::Smart;

    i32 Counter = 0;
    {

        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        CHECK_EQUALS( "Scoped smart.", Counter, 1 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 2 )
    {
        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        CHECK_EQUALS( "Scoped smart.", Counter, 3 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 4 )
    {
        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        CHECK_EQUALS( "Scoped smart.", Counter, 5 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 6 )
    {
        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        Smart::TUnique<LMySmartCounter> UniqueB = std::move(UniqueA);
        CHECK_EQUALS( "Scoped smart.", Counter, 7 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 8 )
    {
        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        Smart::TUnique<LMySmartCounter> UniqueB = std::move(UniqueA);

        CHECK_NULL(   "Scoped smart.", UniqueA.GetPointer() )
        CHECK_EQUALS( "Scoped smart.", UniqueB.GetValue().Counter, &Counter )
        UniqueB.Reset();
        CHECK_EQUALS( "Scoped smart.", Counter, 10 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 10 )
    {
        Smart::TUnique<LMySmartCounter> UniqueA = Smart::EmplaceUnique<LMySmartCounter>(&Counter);
        CHECK_EQUALS( "Scoped smart.", Counter, 11 )
        Smart::TUnique<LMySmartCounter> UniqueB = std::move(UniqueA);
        CHECK_NULL(   "Scoped smart.", UniqueA.GetPointer() )
        CHECK_EQUALS( "Scoped smart.", UniqueB.GetValue().Counter, &Counter )
        CHECK_EQUALS( "Scoped smart.", Counter, 11 )
    }
    CHECK_EQUALS( "Scoped smart.", Counter, 12 )

    return;
}
