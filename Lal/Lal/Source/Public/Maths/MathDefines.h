// Copyright mzoesch. All rights reserved.

#pragma once

/*----------------------------------------------------------------------------
    Floating point constants.
----------------------------------------------------------------------------*/

///////////////////////////////////////////////////////////////////////////////
// Thresholds.
#define JAFG_THRESHOLD_NORM_VEC_F           (1.e-2f) /** Tolerance flt constant for norm check with squared magnitude of a vector. */
#define JAFG_THRESHOLD_NORM_VEC_D           (1.e-2)  /** Tolerance dbl constant for norm check with squared magnitude of a vector. */

///////////////////////////////////////////////////////////////////////////////
// Common constants.
#define JAFG_FLOAT_SMALL_NUMBER             (1.e-8f)
#define JAFG_FLOAT_NOT_SO_SMALL_NUMBER      (1.e-4f)

#define JAFG_DOUBLE_SMALL_NUMBER            (1.e-8)
#define JAFG_DOUBLE_NOT_SO_SMALL_NUMBER     (1.e-4)

#define JAFG_PI_F                           (3.1415926535897932384626433832795028841971693993751058209749445923078164f)
#define JAFG_HALF_PI_F                      (1.5707963267948966192313216916397514420985846996875529104874722961539082f)
#define JAFG_TWO_PI_F                       (6.2831853071795864769252867665590057683943387987502116419498891846156328f)
#define JAFG_PI_SQRT_F                      (1.7724538509055160272981674833411451827975494561223871282138077898529112f)
#define JAFG_PI_SQUARED_F                   (9.8696044010893586188344909998761511353136994072407906264133493762200448f)
#define JAFG_PI_D                           (3.1415926535897932384626433832795028841971693993751058209749445923078164 )
#define JAFG_HALF_PI_D                      (1.5707963267948966192313216916397514420985846996875529104874722961539082 )
#define JAFG_TWO_PI_D                       (6.2831853071795864769252867665590057683943387987502116419498891846156328 )
#define JAFG_PI_SQRT_D                      (1.7724538509055160272981674833411451827975494561223871282138077898529112 )
#define JAFG_PI_SQUARED_D                   (9.8696044010893586188344909998761511353136994072407906264133493762200448 )

#define JAFG_DEG_FULL_CIRCLE_F              (360.0f)
#define JAFG_RAD_FULL_CIRCLE_F              (JAFG_TWO_PI_F)
#define JAFG_DEG_QUARTER_CIRCLE_F           (90.0f)
#define JAFG_RAD_QUARTER_CIRCLE_F           (JAFG_HALF_PI_F)
#define JAFG_DEG_HALF_CIRCLE_F              (180.0f)
#define JAFG_RAD_HALF_CIRCLE_F              (JAFG_PI_F)
#define JAFG_DEG_THREE_THIRD_CIRCLE_F       (270.0f)
#define JAFG_RAD_THREE_THIRD_CIRCLE_F       (4.7123889803846898576939650749192543262957540990626587314624168884617246f)
#define JAFG_DEG_FULL_CIRCLE_D              (360.0)
#define JAFG_RAD_FULL_CIRCLE_D              (JAFG_TWO_PI_D)
#define JAFG_DEG_QUARTER_CIRCLE_D           (90.0)
#define JAFG_RAD_QUARTER_CIRCLE_D           (JAFG_HALF_PI_D)
#define JAFG_DEG_HALF_CIRCLE_D              (180.0)
#define JAFG_RAD_HALF_CIRCLE_D              (JAFG_PI_D)
#define JAFG_DEG_THREE_THIRD_CIRCLE_D       (270.0)
#define JAFG_RAD_THREE_THIRD_CIRCLE_D       (4.7123889803846898576939650749192543262957540990626587314624168884617246 )

#define JAFG_FLOAT_SUPREMUM                 (3.402823466e+38f)          /* Max positive flt value. */
#define JAFG_FLOAT_INFIMUM                  (1.175494351e-38f)          /* Min normalized positive flt value. */
#define JAFG_FLOAT_TRUE_MINIMUM             (1.401298464e-45f)          /* Min positive flt value. */
#define JAFG_DOUBLE_SUPREMUM                (1.7976931348623158e+308)   /* Max positive dbl value. */
#define JAFG_DOUBLE_INFIMUM                 (2.2250738585072014e-308)   /* Min normalized positive dbl value. */
#define JAFG_DOUBLE_TRUE_MINIMUM            (4.9406564584124654e-324)   /* Min positive dbl value. */

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for metric units.
#define JAFG_KM2M_F                         (1e+3f)
#define JAFG_KM2CM_F                        (1e+5f)
#define JAFG_KM2MM_F                        (1e+6f)
#define JAFG_M2KM_F                         (1e-3f)
#define JAFG_M2CM_F                         (1e+2f)
#define JAFG_M2MM_F                         (1e+3f)
#define JAFG_CM2KM_F                        (1e-5f)
#define JAFG_CM2M_F                         (1e-2f)
#define JAFG_CM2MM_F                        (1e+1f)
#define JAFG_MM2KM_F                        (1e-6f)
#define JAFG_MM2M_F                         (1e-3f)
#define JAFG_MM2CM_F                        (1e-1f)

#define JAFG_KM2M_D                         (1e+3)
#define JAFG_KM2CM_D                        (1e+5)
#define JAFG_KM2MM_D                        (1e+6)
#define JAFG_M2KM_D                         (1e-3)
#define JAFG_M2CM_D                         (1e+2)
#define JAFG_M2MM_D                         (1e+3)
#define JAFG_CM2KM_D                        (1e-5)
#define JAFG_CM2M_D                         (1e-2)
#define JAFG_CM2MM_D                        (1e+1)
#define JAFG_MM2KM_D                        (1e-6)
#define JAFG_MM2M_D                         (1e-3)
#define JAFG_MM2CM_D                        (1e-1)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for squared metric units.
#define JAFG_KM2_TO_M2_F                    (1e+6f)
#define JAFG_KM2_TO_CM2_F                   (1e+10f)
#define JAFG_KM2_TO_MM2_F                   (1e+12f)
#define JAFG_M2_TO_KM2_F                    (1e-6f)
#define JAFG_M2_TO_CM2_F                    (1e+4f)
#define JAFG_M2_TO_MM2_F                    (1e+6f)
#define JAFG_CM2_TO_KM2_F                   (1e-10f)
#define JAFG_CM2_TO_M2_F                    (1e-4f)
#define JAFG_CM2_TO_MM2_F                   (1e+2f)
#define JAFG_MM2_TO_KM2_F                   (1e-12f)
#define JAFG_MM2_TO_M2_F                    (1e-6f)
#define JAFG_MM2_TO_CM2_F                   (1e-2f)

#define JAFG_KM2_TO_M2_D                    (1e+6)
#define JAFG_KM2_TO_CM2_D                   (1e+10)
#define JAFG_KM2_TO_MM2_D                   (1e+12)
#define JAFG_M2_TO_KM2_D                    (1e-6)
#define JAFG_M2_TO_CM2_D                    (1e+4)
#define JAFG_M2_TO_MM2_D                    (1e+6)
#define JAFG_CM2_TO_KM2_D                   (1e-10)
#define JAFG_CM2_TO_M2_D                    (1e-4)
#define JAFG_CM2_TO_MM2_D                   (1e+2)
#define JAFG_MM2_TO_KM2_D                   (1e-12)
#define JAFG_MM2_TO_M2_D                    (1e-6)
#define JAFG_MM2_TO_CM2_D                   (1e-2)

///////////////////////////////////////////////////////////////////////////////
// Scalar conversion constants for cubed metric units.
#define JAFG_KM3_TO_M3_F                    (1e+9f)
#define JAFG_KM3_TO_CM3_F                   (1e+15f)
#define JAFG_KM3_TO_MM3_F                   (1e+18f)
#define JAFG_M3_TO_KM3_F                    (1e-9f)
#define JAFG_M3_TO_CM3_F                    (1e+6f)
#define JAFG_M3_TO_MM3_F                    (1e+9f)
#define JAFG_CM3_TO_KM3_F                   (1e-15f)
#define JAFG_CM3_TO_M3_F                    (1e-6f)
#define JAFG_CM3_TO_MM3_F                   (1e+3f)
#define JAFG_MM3_TO_KM3_F                   (1e-18f)
#define JAFG_MM3_TO_M3_F                    (1e-9f)
#define JAFG_MM3_TO_CM3_F                   (1e-3f)

#define JAFG_KM3_TO_M3_D                    (1e+9)
#define JAFG_KM3_TO_CM3_D                   (1e+15)
#define JAFG_KM3_TO_MM3_D                   (1e+18)
#define JAFG_M3_TO_KM3_D                    (1e-9)
#define JAFG_M3_TO_CM3_D                    (1e+6)
#define JAFG_M3_TO_MM3_D                    (1e+9)
#define JAFG_CM3_TO_KM3_D                   (1e-15)
#define JAFG_CM3_TO_M3_D                    (1e-6)
#define JAFG_CM3_TO_MM3_D                   (1e+3)
#define JAFG_MM3_TO_KM3_D                   (1e-18)
#define JAFG_MM3_TO_M3_D                    (1e-9)
#define JAFG_MM3_TO_CM3_D                   (1e-3)


/*----------------------------------------------------------------------------
    Aliases.
----------------------------------------------------------------------------*/

#define JAFG_SMALL_NUMBER                   JAFG_FLOAT_SMALL_NUMBER
#define JAFG_NOT_SO_SMALL_NUMBER            JAFG_FLOAT_NOT_SO_SMALL_NUMBER
#define JAFG_PI                             JAFG_PI_F
#define JAFG_TWO_PI                         JAFG_TWO_PI_F
#define JAFG_HALF_PI                        JAFG_HALF_PI_F
#define JAFG_PI_SQRT                        JAFG_PI_SQRT_F
#define JAFG_PI_SQUARED                     JAFG_PI_SQUARED_F
#define JAFG_DEB_FULL_CIRCLE                JAFG_DEG_FULL_CIRCLE_F
#define JAFG_RAD_FULL_CIRCLE                JAFG_RAD_FULL_CIRCLE_F
#define JAFG_DEB_QUARTER_CIRCLE             JAFG_DEG_QUARTER_CIRCLE_F
#define JAFG_RAD_QUARTER_CIRCLE             JAFG_RAD_QUARTER_CIRCLE_F
#define JAFG_DEB_HALF_CIRCLE                JAFG_DEG_HALF_CIRCLE_F
#define JAFG_RAD_HALF_CIRCLE                JAFG_RAD_HALF_CIRCLE_F
#define JAFG_DEB_THREE_THIRD_CIRCLE         JAFG_DEG_THREE_THIRD_CIRCLE_F
#define JAFG_RAD_THREE_THIRD_CIRCLE         JAFG_RAD_THREE_THIRD_CIRCLE_F
#define JAFG_KM2M                           JAFG_KM2M_F
#define JAFG_KM2CM                          JAFG_KM2CM_F
#define JAFG_KM2MM                          JAFG_KM2MM_F
#define JAFG_M2KM                           JAFG_M2KM_F
#define JAFG_M2CM                           JAFG_M2CM_F
#define JAFG_M2MM                           JAFG_M2MM_F
#define JAFG_CM2KM                          JAFG_CM2KM_F
#define JAFG_CM2M                           JAFG_CM2M_F
#define JAFG_CM2MM                          JAFG_CM2MM_F
#define JAFG_MM2KM                          JAFG_MM2KM_F
#define JAFG_MM2M                           JAFG_MM2M_F
#define JAFG_MM2CM                          JAFG_MM2CM_F
#define JAFG_KM2_TO_M2                      JAFG_KM2_TO_M2_F
#define JAFG_KM2_TO_CM2                     JAFG_KM2_TO_CM2_F
#define JAFG_KM2_TO_MM2                     JAFG_KM2_TO_MM2_F
#define JAFG_M2_TO_KM2                      JAFG_M2_TO_KM2_F
#define JAFG_M2_TO_CM2                      JAFG_M2_TO_CM2_F
#define JAFG_M2_TO_MM2                      JAFG_M2_TO_MM2_F
#define JAFG_CM2_TO_KM2                     JAFG_CM2_TO_KM2_F
#define JAFG_CM2_TO_M2                      JAFG_CM2_TO_M2_F
#define JAFG_CM2_TO_MM2                     JAFG_CM2_TO_MM2_F
#define JAFG_MM2_TO_KM2                     JAFG_MM2_TO_KM2_F
#define JAFG_MM2_TO_M2                      JAFG_MM2_TO_M2_F
#define JAFG_MM2_TO_CM2                     JAFG_MM2_TO_CM2_F
#define JAFG_KM2_TO_M3                      JAFG_KM3_TO_M3_F
#define JAFG_KM2_TO_CM3                     JAFG_KM3_TO_CM3_F
#define JAFG_KM2_TO_MM3                     JAFG_KM3_TO_MM3_F
#define JAFG_M2_TO_KM3                      JAFG_M3_TO_KM3_F
#define JAFG_M2_TO_CM3                      JAFG_M3_TO_CM3_F
#define JAFG_M2_TO_MM3                      JAFG_M3_TO_MM3_F
#define JAFG_CM2_TO_KM3                     JAFG_CM3_TO_KM3_F
#define JAFG_CM2_TO_M3                      JAFG_CM3_TO_M3_F
#define JAFG_CM2_TO_MM3                     JAFG_CM3_TO_MM3_F
#define JAFG_MM2_TO_KM3                     JAFG_MM3_TO_KM3_F
#define JAFG_MM2_TO_M3                      JAFG_MM3_TO_M3_F
#define JAFG_MM2_TO_CM3                     JAFG_MM3_TO_CM3_F
