// Copyright mzoesch. All rights reserved.

#pragma once

#include "TestInclude.h"

TEST_CASE(MiscUtilOperations, "Lal.Maths")
{
    using namespace Jafg;

    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(1.0f),                                        1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(-1.0f),                                       1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(1.0),                                          1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(-1.0),                                         1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(1),                                              1 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(-1),                                             1 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(1.0L),                                        1.0L )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Absolute(-1.0L),                                       1.0L )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(1.0f, 2.0f),                                       1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(2.0f, 1.0f),                                       1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_TRUE_MINIMUM, 1.0f), JAFG_FLOAT_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_TRUE_MINIMUM, -1.0f),                  -1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_TRUE_MINIMUM, JAFG_FLOAT_TRUE_MINIMUM),    JAFG_FLOAT_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_TRUE_MINIMUM, JAFG_FLOAT_INFIMUM),         JAFG_FLOAT_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_INFIMUM, JAFG_FLOAT_SUPREMUM),                  JAFG_FLOAT_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_FLOAT_SUPREMUM, JAFG_FLOAT_INFIMUM),                  JAFG_FLOAT_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(1.0, 2.0),                                          1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(2.0, 1.0),                                          1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_TRUE_MINIMUM, 1.0),                      JAFG_DOUBLE_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_TRUE_MINIMUM, -1.0),                   -1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_TRUE_MINIMUM, JAFG_DOUBLE_TRUE_MINIMUM), JAFG_DOUBLE_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_TRUE_MINIMUM, JAFG_DOUBLE_INFIMUM),      JAFG_DOUBLE_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_INFIMUM, JAFG_DOUBLE_SUPREMUM),               JAFG_DOUBLE_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Min(JAFG_DOUBLE_SUPREMUM, JAFG_DOUBLE_INFIMUM),               JAFG_DOUBLE_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(1.0f, 2.0f),                                       2.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(2.0f, 1.0f),                                       2.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_TRUE_MINIMUM, 1.0f),                    1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_TRUE_MINIMUM, -1.0f),                      JAFG_FLOAT_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_TRUE_MINIMUM, JAFG_FLOAT_TRUE_MINIMUM),    JAFG_FLOAT_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_TRUE_MINIMUM, JAFG_FLOAT_INFIMUM),              JAFG_FLOAT_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_INFIMUM, JAFG_FLOAT_SUPREMUM),                 JAFG_FLOAT_SUPREMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_FLOAT_SUPREMUM, JAFG_FLOAT_INFIMUM),                 JAFG_FLOAT_SUPREMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(1.0, 2.0),                                          2.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(2.0, 1.0),                                          2.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_TRUE_MINIMUM, 1.0),                     1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_TRUE_MINIMUM, -1.0),                     JAFG_DOUBLE_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_TRUE_MINIMUM, JAFG_DOUBLE_TRUE_MINIMUM), JAFG_DOUBLE_TRUE_MINIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_TRUE_MINIMUM, JAFG_DOUBLE_INFIMUM),           JAFG_DOUBLE_INFIMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_INFIMUM, JAFG_DOUBLE_SUPREMUM),              JAFG_DOUBLE_SUPREMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Max(JAFG_DOUBLE_SUPREMUM, JAFG_DOUBLE_INFIMUM),              JAFG_DOUBLE_SUPREMUM )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(1.0f, 2.0f, 3.0f),                               2.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(2.0f, 1.0f, 3.0f),                               2.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(3.0f, 1.0f, 2.0f),                               2.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(-100.0f, -1.0f, 1.0f),                          -1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(1.0f, -1.0f, 1.0f),                              1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(100.0f, -1.0f, 1.0f),                            1.0f )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(1.0, 2.0, 3.0),                                   2.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(2.0, 1.0, 3.0),                                   2.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(3.0, 1.0, 2.0),                                   2.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(-100.0, -1.0, 1.0),                              -1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(1.0, -1.0, 1.0),                                  1.0 )
    CHECK_EQUALS( "MiscUtilOperations", Maths::Clamp(100.0, -1.0, 1.0),                                1.0 )

    return;
}

TEST_CASE(TrigonometryOperations, "Lal.Maths")
{
    using namespace Jafg;

    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( 0.0f),                              0 * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-0.0f),                              0 * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_QUARTER_CIRCLE_F),        JAFG_HALF_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_QUARTER_CIRCLE_F),       -JAFG_HALF_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_HALF_CIRCLE_F),                JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_HALF_CIRCLE_F),               -JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_THREE_THIRD_CIRCLE_F),  1.5f * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_THREE_THIRD_CIRCLE_F), -1.5f * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_FULL_CIRCLE_F),            JAFG_TWO_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_FULL_CIRCLE_F),           -JAFG_TWO_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( 0.0f),                 0 * JAFG_DEG_FULL_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-0.0f),                 0 * JAFG_DEG_FULL_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_HALF_PI_F),        JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_HALF_PI_F),       -JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_PI_F),                JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_PI_F),               -JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( 1.5f * JAFG_PI_F),  JAFG_DEG_THREE_THIRD_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-1.5f * JAFG_PI_F), -JAFG_DEG_THREE_THIRD_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_TWO_PI_F),            JAFG_DEG_FULL_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_TWO_PI_F),           -JAFG_DEG_FULL_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( 0.0),                               0 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-0.0),                               0 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_QUARTER_CIRCLE_D),        JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_QUARTER_CIRCLE_D),       -JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_HALF_CIRCLE_D),                JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_HALF_CIRCLE_D),               -JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_THREE_THIRD_CIRCLE_D),   1.5 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_THREE_THIRD_CIRCLE_D),  -1.5 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians( JAFG_DEG_FULL_CIRCLE_D),            JAFG_TWO_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToRadians(-JAFG_DEG_FULL_CIRCLE_D),           -JAFG_TWO_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( 0.0),                  0 * JAFG_DEG_FULL_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-0.0),                  0 * JAFG_DEG_FULL_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_HALF_PI_D),        JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_HALF_PI_D),       -JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_PI_D),                JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_PI_D),               -JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( 1.5 * JAFG_PI_D),   JAFG_DEG_THREE_THIRD_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-1.5 * JAFG_PI_D),  -JAFG_DEG_THREE_THIRD_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees( JAFG_TWO_PI_D),            JAFG_DEG_FULL_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ToDegrees(-JAFG_TWO_PI_D),           -JAFG_DEG_FULL_CIRCLE_D )

    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(0.0f),                              0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_FULL_CIRCLE_F),            0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(2.0f * JAFG_DEG_FULL_CIRCLE_F),     0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_FULL_CIRCLE_F),           0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-2.0f * JAFG_DEG_FULL_CIRCLE_F),    0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_HALF_CIRCLE_F),               JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(2.0f * JAFG_DEG_HALF_CIRCLE_F),     0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_HALF_CIRCLE_F),              JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-2.0f * JAFG_DEG_HALF_CIRCLE_F),    0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_QUARTER_CIRCLE_F),         JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_QUARTER_CIRCLE_F),    JAFG_DEG_THREE_THIRD_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_THREE_THIRD_CIRCLE_F), JAFG_DEG_THREE_THIRD_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_THREE_THIRD_CIRCLE_F),    JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(0.0f),                          0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_FULL_CIRCLE_F),        0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(2.0f * JAFG_DEG_FULL_CIRCLE_F), 0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_FULL_CIRCLE_F),       0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-2.0f * JAFG_DEG_FULL_CIRCLE_F),    0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_HALF_CIRCLE_F),            JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(2.0f * JAFG_DEG_HALF_CIRCLE_F),     0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_HALF_CIRCLE_F),           JAFG_DEG_HALF_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-2.0f * JAFG_DEG_HALF_CIRCLE_F),    0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_QUARTER_CIRCLE_F),      JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_QUARTER_CIRCLE_F),    -JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_THREE_THIRD_CIRCLE_F), -JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_THREE_THIRD_CIRCLE_F), JAFG_DEG_QUARTER_CIRCLE_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(0.0),                                0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_FULL_CIRCLE_D),             0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(2.0 * JAFG_DEG_FULL_CIRCLE_D),       0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_FULL_CIRCLE_D),            0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-2.0 * JAFG_DEG_FULL_CIRCLE_D),      0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_HALF_CIRCLE_D),               JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(2.0 * JAFG_DEG_HALF_CIRCLE_D),       0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_HALF_CIRCLE_D),              JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-2.0 * JAFG_DEG_HALF_CIRCLE_D),      0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_QUARTER_CIRCLE_D),         JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_QUARTER_CIRCLE_D),    JAFG_DEG_THREE_THIRD_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(JAFG_DEG_THREE_THIRD_CIRCLE_D), JAFG_DEG_THREE_THIRD_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampDegrees(-JAFG_DEG_THREE_THIRD_CIRCLE_D),    JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(0.0),                             0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_FULL_CIRCLE_D),         0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(2.0 * JAFG_DEG_FULL_CIRCLE_D),   0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_FULL_CIRCLE_D),        0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-2.0 * JAFG_DEG_FULL_CIRCLE_D),     0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_HALF_CIRCLE_D),            JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(2.0 * JAFG_DEG_HALF_CIRCLE_D),      0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_HALF_CIRCLE_D),           JAFG_DEG_HALF_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-2.0 * JAFG_DEG_HALF_CIRCLE_D),     0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_QUARTER_CIRCLE_D),      JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_QUARTER_CIRCLE_D),    -JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(JAFG_DEG_THREE_THIRD_CIRCLE_D), -JAFG_DEG_QUARTER_CIRCLE_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeDegrees(-JAFG_DEG_THREE_THIRD_CIRCLE_D), JAFG_DEG_QUARTER_CIRCLE_D )

    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(0.0f),                               0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_TWO_PI_F),                      0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(2.0f * JAFG_TWO_PI_F),               0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_TWO_PI_F),            JAFG_TWO_PI_F ) // FLT ERROR - Watch out.
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-2.0f * JAFG_TWO_PI_F),     JAFG_TWO_PI_F, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_PI_F),                     JAFG_PI_F ) // FLT ERROR - We should probably do some
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(2.0f * JAFG_PI_F),                   0.0f ) //             precision checks when calculating
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_PI_F),                    JAFG_PI_F ) //             the clamp values with low precision
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-2.0f * JAFG_PI_F),         JAFG_TWO_PI_F ) //             values - e.g. floats.
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_HALF_PI_F),           JAFG_HALF_PI_F ) // FLT ERROR
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_HALF_PI_F),        1.5f * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(1.5f * JAFG_PI_F),       1.5f * JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-1.5f * JAFG_PI_F),        JAFG_HALF_PI_F, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(0.0f),                           0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_TWO_PI_F),                  0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(2.0f * JAFG_TWO_PI_F),           0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_TWO_PI_F),                 0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-2.0f * JAFG_TWO_PI_F),          0.0f, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_PI_F),                 JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(2.0f * JAFG_PI_F),               0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_PI_F),                JAFG_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-2.0f * JAFG_PI_F),              0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_HALF_PI_F),       JAFG_HALF_PI_F )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_HALF_PI_F),     -JAFG_HALF_PI_F, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(1.5f * JAFG_PI_F),    -JAFG_HALF_PI_F, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-1.5f * JAFG_PI_F),    JAFG_HALF_PI_F, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(0.0),                                 0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_TWO_PI_D),                       0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(2.0 * JAFG_TWO_PI_D),                 0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_TWO_PI_D),                      0.0 ) // DBL OK
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-2.0 * JAFG_TWO_PI_D),                0.0 ) // DBL OK
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_PI_D),                     JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(2.0 * JAFG_PI_D),                     0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_PI_D),                    JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-2.0 * JAFG_PI_D),                    0.0 ) // DBL OK
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(JAFG_HALF_PI_D),           JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-JAFG_HALF_PI_D),         1.5 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(1.5 * JAFG_PI_D),         1.5 * JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::ClampRadians(-1.5 * JAFG_PI_D),         JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(0.0),                             0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_TWO_PI_D),                   0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(2.0 * JAFG_TWO_PI_D),             0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_TWO_PI_D),                  0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-2.0 * JAFG_TWO_PI_D),            0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_PI_D),                 JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(2.0 * JAFG_PI_D),                 0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_PI_D),                JAFG_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-2.0 * JAFG_PI_D),                0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(JAFG_HALF_PI_D),       JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-JAFG_HALF_PI_D),     -JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(1.5 * JAFG_PI_D),     -JAFG_HALF_PI_D )
    CHECK_EQUALS( "TrigonometryOperations", Maths::NormalizeRadians(-1.5 * JAFG_PI_D),     JAFG_HALF_PI_D )

    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(0.0f),                              0.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_HALF_PI_F),                    1.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_PI_F),                         0.0f, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(1.5f * JAFG_PI_F),                 -1.0f )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_TWO_PI_F),                     0.0f, JAFG_FLOAT_NOT_SO_SMALL_NUMBER )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(0.0),                                0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_HALF_PI_D),                     1.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_PI_D),                          0.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(1.5 * JAFG_PI_D),                   -1.0 )
    CHECK_EQUALS( "TrigonometryOperations", Maths::Sin(JAFG_TWO_PI_D),                      0.0 )

    return;
}

TEST_CASE(MatrixOperations, "Lal.Maths")
{
    using namespace Jafg;

    const LVector E = LVector(0.0f, 0.0f, 5.0f);
    const LVector F = LVector(1.0f, 2.0f, 0.0f);
    const LVector U = LVector(0.0f, 0.0f, 1.0f);
    const LMatrix V = Maths::MakeViewMatrix(E, E + F, U);
    const LMatrix Expected =
        {
            LPlaneF{  0.894427299f, -0.0f,        -0.44721359f, 0.0f },
            LPlaneF{ -0.44721365f,   0.0f,        -0.89442718f, 0.0f },
            LPlaneF{  0.0f,          1.00000012f, -0.0f,        0.0f },
            LPlaneF{ -0.0f,         -5.00000048f,  0.0f,        1.0f }
        };

    CHECK_EQUALS( "MatrixOperations", V, Expected )

    return;
}
