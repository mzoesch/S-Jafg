// Copyright mzoesch. All rights reserved.

#pragma once

#include "Widgets/InputRange.h"

TRIVIAL_TEST_CASE("widgets", InputRangeClamp)
{
    using namespace Jafg;

    LInputRange const Unit{.Min = 0.0, .Max = 1.0};
    TEST_EQUAL(Unit.Clamp(0.5), 0.5)
    TEST_EQUAL(Unit.Clamp(-0.25), 0.0)
    TEST_EQUAL(Unit.Clamp(1.5f), 1.0f)

    /* Integrals are clamped to the closest integral inside of the bounds. */
    LInputRange const Fractional{.Min = -2.5, .Max = 7.5};
    TEST_EQUAL(Fractional.Clamp(i64{-10}), i64{-2})
    TEST_EQUAL(Fractional.Clamp(i64{10}), i64{7})
    TEST_EQUAL(Fractional.Clamp(i64{3}), i64{3})
    /* A negative min does not wrap around for unsigned values. */
    TEST_EQUAL(Fractional.Clamp(u64{0}), u64{0})
    TEST_EQUAL(Fractional.Clamp(u64{100}), u64{7})
    TEST_EQUAL(LInputRange{.Min = 10.0}.Clamp(u8{3}), u8{10})
    TEST_EQUAL(LInputRange{.Max = 1.0e30}.Clamp(u64{5}), u64{5})
    TEST_EQUAL(LInputRange{.Min = 1.0e30}.Clamp(u64{5}), std::numeric_limits<u64>::max())

    /* Each bound is optional on its own. */
    TEST_EQUAL(LInputRange{.Min = 0.0}.Clamp(-1.0), 0.0)
    TEST_EQUAL(LInputRange{.Min = 0.0}.Clamp(1000.0), 1000.0)
    TEST_EQUAL(LInputRange{}.Clamp(-1000.0), -1000.0)
}

TRIVIAL_TEST_CASE("widgets", InputRangeFraction)
{
    using namespace Jafg;

    LInputRange const Range{.Min = 2.0, .Max = 6.0};
    TEST_TRUE(Range.IsBounded())
    TEST_EQUAL(Range.GetFraction(2.0).value_or(-1.0f), 0.0f)
    TEST_EQUAL(Range.GetFraction(4.0).value_or(-1.0f), 0.5f)
    TEST_EQUAL(Range.GetFraction(6.0).value_or(-1.0f), 1.0f)
    /* Values out of the bounds (e.g., the initial value) stay at the ends of the bar. */
    TEST_EQUAL(Range.GetFraction(-3.0).value_or(-1.0f), 0.0f)
    TEST_EQUAL(Range.GetFraction(9.0).value_or(-1.0f), 1.0f)

    /* Only both bounds that span a range show a bar. */
    TEST_FALSE(LInputRange{.Min = 0.0}.GetFraction(0.5).has_value())
    TEST_FALSE(LInputRange{.Max = 1.0}.GetFraction(0.5).has_value())
    TEST_FALSE((LInputRange{.Min = 1.0, .Max = 1.0}).GetFraction(1.0).has_value())
    TEST_FALSE(LInputRange{}.IsBounded())
}
